# Something Strange

A story-driven first-person ghost hunting horror game, built in Unreal Engine 5 with assets made in Tripo 3D and Blender.

Ned is a teenage genius. One of his experiments tears open the spirit realm, lets ghosts and demons into the world, and takes his mom. With a laser he built himself and goggles that let him see on the ghosts' frequency for a few seconds at a time, he has to send them back and bring her home.

## Documents

| Document | Contents |
|---|---|
| [docs/GDD.md](docs/GDD.md) | Game design: mechanics, entities, progression |
| [docs/Story.md](docs/Story.md) | Story outline and the Level 1 beat sheet |
| [docs/Script.md](docs/Script.md) | Prologue script, including the tutorial |
| [docs/Audio.md](docs/Audio.md) | Audio plan, noise, and how entities sense Ned |
| [docs/LevelMetrics.md](docs/LevelMetrics.md) | Standard dimensions and rules for blocking out the house |
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
| **SS Pickup** | Set **Type** to Goggles, Laser, Residue, or Health. To make Ned find his equipment, place Goggles and Laser pickups, drag an **SS Player Character** into the level, set its **Auto Possess Player** to Player 0, and untick its **Start With Equipment**. |
| **SS Power Switch** | Fires an event when switched on, for the level Blueprint to turn the lights back on. |

If Ned's health reaches zero, the level restarts after a moment.

### Setting up the prologue

The prologue in [docs/Script.md](docs/Script.md) is run by one actor, **SS Prologue Director**. It sets the objectives, shows the control hints, plays the subtitled lines, and moves on as the player does each thing. Place the actors below in a level, then select the director and point each of its properties at them.

| Director property | Actor to place | Settings on that actor |
|---|---|---|
| Front Door | SS Door | Put the player start outside it, facing it |
| (Shoulder Item Id) | SS Pickup, in the freezer | Type: Part. Item Id: `PorkShoulder`. Display Name: pork shoulder |
| Lab Trigger | Trigger Box at the bottom of the basement stairs | |
| Goggles Workbench | SS Workbench | Result: Goggles. Result Name: goggles. Required Items: `LensArray`, `TuningCoil`, `PowerCell` |
| | Three SS Pickups around the basement | Type: Part. Item Ids as above, with Display Names |
| Laser Workbench | SS Workbench | Result: Laser. Result Name: emitter. Required Items: empty |
| Paint Can | SS Tutorial Target | Merged: on. Indestructible: off |
| Meat | SS Tutorial Target | Merged: on. Indestructible: on. Start Active: off |
| Specimen | SS Tutorial Target, on the goggles bench | Merged: off. Indestructible: on |
| Aperture Switches | Three SS Power Switches | |

Any property left empty is skipped, so the sequence can be tested with only some of the actors placed. If there is no freezer pickup yet, clear **Shoulder Item Id** on the director, or the sequence will wait for it.

When the last switch is thrown the director fires **On Prologue Finished**. Nothing happens after that yet: the cutscene of Mom being taken and the start of Level 1 hook in there.

The lines of dialogue are in `Source/SomethingStrange/Story/SSPrologueDirector.cpp`, as plain text near the middle of the file.

## Repository layout

```
SomethingStrange.uproject
Config/                    Project settings
Source/SomethingStrange/   C++ game module
Content/                   Unreal assets (Git LFS)
Tools/blender/             Asset cleanup script
docs/                      Design documents
```
