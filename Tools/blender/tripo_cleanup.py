"""Clean up Tripo 3D exports and write Unreal-ready FBX files.

Run headless, one FBX per input GLB:

    blender --background --python Tools/blender/tripo_cleanup.py -- \
        --input SourceArt/Tripo --output SourceArt/Export --tris 5000

For each .glb in the input folder this:
  1. imports it and joins its meshes into one object;
  2. applies transforms and moves the origin to the centre of the base;
  3. merges duplicate vertices and recalculates normals;
  4. decimates down to the triangle budget, if it is over;
  5. builds a convex collision hull named UCX_<name>_00 (skip with --no-collision);
  6. exports SM_<Name>.fbx with textures embedded.

This is for static props. Characters and first-person weapons need manual
retopology and rigging; see docs/AssetPipeline.md.
"""

import argparse
import re
import sys
from pathlib import Path

import bpy  # first: when run as the pip "bpy" module, this makes bmesh and mathutils importable
import bmesh
from mathutils import Vector

MERGE_DISTANCE = 0.0001  # metres
COLLISION_MAX_FACES = 64


def parse_args():
    # Blender passes everything after "--" through to the script.
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--input", required=True, type=Path, help="Folder of .glb files from Tripo")
    parser.add_argument("--output", required=True, type=Path, help="Folder to write .fbx files to")
    parser.add_argument("--tris", type=int, default=5000, help="Triangle budget per asset (default 5000)")
    parser.add_argument("--no-collision", action="store_true", help="Do not generate a UCX collision hull")
    return parser.parse_args(argv)


def asset_name(path):
    """'old wooden-chair.glb' -> 'OldWoodenChair'."""
    words = re.split(r"[^0-9A-Za-z]+", path.stem)
    return "".join(word[:1].upper() + word[1:] for word in words if word)


def reset_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)


def select_only(obj):
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj


def import_and_join(path):
    bpy.ops.import_scene.gltf(filepath=str(path))
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    if not meshes:
        raise RuntimeError("no mesh objects in file")

    bpy.ops.object.select_all(action="DESELECT")
    for obj in meshes:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    if len(meshes) > 1:
        bpy.ops.object.join()

    obj = bpy.context.view_layer.objects.active
    # Detach from any imported empties, keeping the world transform, then bake it in.
    bpy.ops.object.parent_clear(type="CLEAR_KEEP_TRANSFORM")
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    return obj


def move_origin_to_base(obj):
    """Shift the mesh so the origin is at the centre of its base."""
    corners = [Vector(corner) for corner in obj.bound_box]
    low = Vector((min(c.x for c in corners), min(c.y for c in corners), min(c.z for c in corners)))
    high = Vector((max(c.x for c in corners), max(c.y for c in corners), max(c.z for c in corners)))
    offset = Vector(((low.x + high.x) / 2, (low.y + high.y) / 2, low.z))

    for vertex in obj.data.vertices:
        vertex.co -= offset
    obj.data.update()
    obj.location = (0, 0, 0)


def clean_mesh(obj):
    mesh = bmesh.new()
    mesh.from_mesh(obj.data)
    bmesh.ops.remove_doubles(mesh, verts=mesh.verts, dist=MERGE_DISTANCE)
    bmesh.ops.recalc_face_normals(mesh, faces=mesh.faces)
    mesh.to_mesh(obj.data)
    mesh.free()
    obj.data.update()


def triangle_count(obj):
    obj.data.calc_loop_triangles()
    return len(obj.data.loop_triangles)


def decimate_to_budget(obj, budget):
    before = triangle_count(obj)
    if before <= budget:
        return before, before

    select_only(obj)
    modifier = obj.modifiers.new(name="Budget", type="DECIMATE")
    modifier.ratio = budget / before
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    return before, triangle_count(obj)


def build_collision(obj, name):
    """Convex hull of the mesh, named the way Unreal expects for custom collision."""
    hull_mesh = bpy.data.meshes.new(f"UCX_{name}_00")
    # Hull the bare vertices: hulling the full mesh would stack new faces on the old ones.
    mesh = bmesh.new()
    for vertex in obj.data.vertices:
        mesh.verts.new(vertex.co)
    result = bmesh.ops.convex_hull(mesh, input=mesh.verts)
    # Keep only the hull: drop vertices that ended up inside it.
    leftovers = {e for e in result["geom_interior"] + result["geom_unused"] if isinstance(e, bmesh.types.BMVert)}
    bmesh.ops.delete(mesh, geom=list(leftovers), context="VERTS")
    mesh.to_mesh(hull_mesh)
    mesh.free()

    hull = bpy.data.objects.new(f"UCX_{name}_00", hull_mesh)
    bpy.context.scene.collection.objects.link(hull)

    faces = len(hull_mesh.polygons)
    if faces > COLLISION_MAX_FACES:
        select_only(hull)
        modifier = hull.modifiers.new(name="Simplify", type="DECIMATE")
        modifier.ratio = COLLISION_MAX_FACES / faces
        bpy.ops.object.modifier_apply(modifier=modifier.name)
    return hull


def export_fbx(objects, path):
    bpy.ops.object.select_all(action="DESELECT")
    for obj in objects:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = objects[0]
    bpy.ops.export_scene.fbx(
        filepath=str(path),
        use_selection=True,
        object_types={"MESH"},
        mesh_smooth_type="FACE",
        path_mode="COPY",
        embed_textures=True,
    )


def process(path, output_dir, budget, with_collision):
    reset_scene()
    name = asset_name(path)

    obj = import_and_join(path)
    obj.name = f"SM_{name}"
    obj.data.name = f"SM_{name}"

    clean_mesh(obj)
    before, after = decimate_to_budget(obj, budget)
    move_origin_to_base(obj)

    exported = [obj]
    if with_collision:
        exported.append(build_collision(obj, f"SM_{name}"))

    target = output_dir / f"SM_{name}.fbx"
    export_fbx(exported, target)
    print(f"[tripo_cleanup] {path.name}: {before} -> {after} tris, wrote {target}")


def main():
    args = parse_args()
    sources = sorted(args.input.glob("*.glb"))
    if not sources:
        print(f"[tripo_cleanup] no .glb files in {args.input}")
        return 1

    args.output.mkdir(parents=True, exist_ok=True)
    failures = 0
    for path in sources:
        try:
            process(path, args.output, args.tris, not args.no_collision)
        except Exception as error:  # keep going so one bad file does not stop the batch
            failures += 1
            print(f"[tripo_cleanup] FAILED {path.name}: {error}")

    print(f"[tripo_cleanup] {len(sources) - failures} of {len(sources)} exported")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
