# Asset pipeline

Tripo 3D generates, Blender cleans up, Unreal Engine 5 imports.

## Static props

1. **Generate in Tripo.** Export as GLB with PBR textures. Save the files in one folder, named for what they are (`kitchen chair.glb`).
2. **Clean up in Blender** with the batch script:

   ```
   blender --background --python Tools/blender/tripo_cleanup.py -- \
       --input SourceArt/Tripo --output SourceArt/Export --tris 5000
   ```

   For each GLB it joins the meshes, applies transforms, puts the origin at the centre of the base, merges duplicate vertices, recalculates normals, decimates to the triangle budget, builds a convex collision hull (`UCX_<name>_00`, 64 faces at most), and writes `SM_<Name>.fbx` with textures embedded.

   | Option | Meaning |
   |---|---|
   | `--tris N` | Triangle budget per asset. Default 5000 |
   | `--no-collision` | Skip the collision hull |

3. **Import into UE5** under `Content/SomethingStrange/Art/<Category>/`. Unreal picks up the `UCX_` mesh as collision automatically.

The script was tested with Blender 5.3 on a generated model: 24,332 triangles reduced to 2,000, origin at the base, 64-face hull. It has not yet been run on a real Tripo export; check the first few in Unreal for scale, orientation, and texture assignment.

A single convex hull is wrong for hollow or concave props (a table, a doorway). For those, pass `--no-collision` and build collision in Unreal.

## Triangle budgets

| Category | Budget |
|---|---|
| Small props (cups, books, tools) | 500 to 1,500 |
| Furniture | 2,000 to 6,000 |
| Hero props seen up close | 8,000 to 15,000 |

With Nanite enabled these matter less for static meshes, but lower counts keep files small and editing fast.

## Naming

| Prefix | Type |
|---|---|
| `SM_` | Static mesh |
| `SK_` | Skeletal mesh |
| `T_` | Texture, with a suffix: `_D` colour, `_N` normal, `_ORM` occlusion, roughness, metallic |
| `M_`, `MI_` | Material, material instance |
| `BP_` | Blueprint |

## What needs manual work

- **Entities.** Part targeting needs each entity rigged to a skeleton, with each body part (head, arms, legs, torso, core) on its own material slot so it can be textured and merged independently. In Blender: retopologise the Tripo mesh, rig it, assign one material slot per part, and author two texture sets (solid and spectral). In Unreal the skeletal mesh needs a physics asset so the laser can hit individual bones. This is the most labour-heavy asset type in the game; keep the entity count low.
- **The laser and Ned's arms.** They fill the screen in first person and need clean topology, separate moving parts (dial, vents), and correct pivots. Treat Tripo output as a starting block.
- **Mom and Ned.** Keeping Ned in first person throughout and Mom's screen time short saves the most character work.
- **The house itself.** Build walls, floors, stairs, and doors as modular pieces in Blender or with Unreal's modelling tools. Tripo fills the rooms with props.

## Consistency

Generated assets drift in style. Before making props in bulk, fix an art bible: palette, level of wear, texel density, and one shared master material that all props use through instances.

## Source files

Keep Tripo exports and `.blend` files under `SourceArt/` in the repository. They are tracked with Git LFS along with everything in `Content/`.
