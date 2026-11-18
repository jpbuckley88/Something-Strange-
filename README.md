# Something Strange

A story-driven first-person ghost hunting horror game, built in Unreal Engine 5 with assets made in Tripo 3D and Blender.

Ned is a teenage genius. One of his experiments tears open the spirit realm, lets ghosts and demons into the world, and takes his mom. With a laser he built himself and goggles that let him see on the ghosts' frequency for a few seconds at a time, he has to send them back and bring her home.

## Documents

| Document | Contents |
|---|---|
| [docs/GDD.md](docs/GDD.md) | Game design: mechanics, entities, progression |
| [docs/Story.md](docs/Story.md) | Story outline and the Level 1 beat sheet |
| [docs/TechnicalDesign.md](docs/TechnicalDesign.md) | Code architecture and what exists so far |
| [docs/AssetPipeline.md](docs/AssetPipeline.md) | Tripo 3D to Blender to UE5 workflow |
| [docs/Roadmap.md](docs/Roadmap.md) | Milestones |

## Getting started

Requirements: Unreal Engine 5.6 or newer with C++ build tools (Visual Studio on Windows, Xcode on macOS), and Git LFS.

1. Run `git lfs install` once, then clone the repository.
2. If your engine is not 5.6, right-click `SomethingStrange.uproject` and choose **Switch Unreal Engine version**.
3. Right-click `SomethingStrange.uproject` and choose **Generate project files**, then open it and let it build.

## Trying the prototype

There are no content assets yet, so the first test uses a stand-in built from engine shapes.

1. Create a new **Basic** level and save it under `Content/SomethingStrange/Maps/`.
2. In the **Place Actors** panel, search for **SS Test Dummy** and drag one in, about 120 units above the floor.
3. Press **Play**.

| Input | Action |
|---|---|
| W A S D, mouse | Move, look |
| Space | Jump |
| Left mouse | Fire the laser |
| Q | Switch laser frequency |
| Right mouse | Toggle the goggles |
| E | Interact |

What to expect from the dummy:

- The **orange arm** has merged into our world. The orange (material) beam destroys it quickly; the blue (spectral) beam barely scratches it.
- The **leg** starts blue and turns orange after 20 seconds, when it finishes merging. The frequency it is weak to changes with it.
- The **core** on top is invisible until the goggles are on. Destroy it with the spectral beam to banish the dummy.
- Holding fire fills the heat bar; at full the laser cuts out until it cools.

### More things to place

All of these are in the **Place Actors** panel and work with no content assets.

| Actor | What it does |
|---|---|
| **SS Wisp** | A small flying enemy. Its orange maw is what bites: destroy it with the material beam and the wisp is harmless. Its core shows only through the goggles; destroy that with the spectral beam to banish it. It drops residue. |
| **SS Rift** and **SS Rift Anchor** | Place a rift and a few anchors, then select the rift and add the anchors to its **Anchors** list and **SS Wisp** to its **Entity Classes**. The rift spawns wisps until every anchor is destroyed. Anchors show only through the goggles and take the spectral beam. |
| **SS Door** | Opens and closes with E. Tick **Locked** to lock it. |
| **SS Pickup** | Set **Type** to Goggles, Laser, Residue, or Health. To make Ned find his equipment, place Goggles and Laser pickups and untick **Start With Equipment** on the player. |
| **SS Power Switch** | Fires an event when switched on, for the level Blueprint to turn the lights back on. |

If Ned's health reaches zero, the level restarts after a moment.

## Repository layout

```
SomethingStrange.uproject
Config/                    Project settings
Source/SomethingStrange/   C++ game module
Content/                   Unreal assets (Git LFS)
Tools/blender/             Asset cleanup script
docs/                      Design documents
```
