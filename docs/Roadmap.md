# Roadmap

| # | Milestone | Done when | Status |
|---|---|---|---|
| M0 | Repo and project skeleton | Project opens and compiles in UE5; docs committed | Code and docs committed; first compile pending |
| M1 | Greybox house, movement, interaction | Walk a blockout of the whole house; open doors, pick things up | Movement and the interaction system are written; needs the blockout, doors, and pickups |
| M2 | Goggles and laser | Goggles reveal hidden test objects on a charge; laser fires, switches frequency, and overheats; a test dummy with merged and unmerged parts takes increased damage only from the matched frequency | Written, with the test dummy; needs playtesting and tuning |
| M3 | Entities | Wisp, poltergeist, and shade behave as designed; parts can be destroyed individually, and merging progresses over time | Not started |
| M4 | Level 1 playable in greybox | Prologue to rift sealed, with objectives, checkpoints, and the boss | Not started |
| M5 | Art, audio, and cutscenes | Tripo and Blender assets replace the greybox; lighting, sound, and the two story scenes are in | Not started |
| M6 | Lab hub and upgrades | Residue is spent on upgrades that carry into the next level | Not started |

M4 is the decision point. If the greybox level is not tense and fun to play, change the design before spending on art or planning Level 2.

## Next steps

1. Build the project in UE5 and fix any compile errors.
2. Play the test dummy (see the README) and tune charge, heat, and damage values.
3. Block out the house from the room list in [Story.md](Story.md).
4. Add the first interactables: door, pickup, power switch.
5. Start the entity base class and the wisp.

Art can run in parallel from step 3: fix the art bible, then generate and clean up kitchen and living room props.
