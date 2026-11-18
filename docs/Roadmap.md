# Roadmap

| # | Milestone | Done when | Status |
|---|---|---|---|
| M0 | Repo and project skeleton | Project opens and compiles in UE5; docs committed | Code and docs committed; first compile pending |
| M1 | Greybox house, movement, interaction | Walk a blockout of the whole house; open doors, pick things up | Movement, interaction, door, pickups, and power switch are written; needs the house blockout |
| M2 | Goggles and laser | Goggles reveal hidden test objects on a charge; laser fires, switches frequency, and overheats; a test dummy with merged and unmerged parts takes increased damage only from the matched frequency | Written, with the test dummy; needs playtesting and tuning |
| M3 | Entities | Wisp, poltergeist, and shade behave as designed; parts can be destroyed individually, and merging progresses over time | Entity base class and wisp written; poltergeist and shade to do |
| M4 | Level 1 playable in greybox | Prologue to rift sealed, with objectives, checkpoints, and the boss | Rift, anchors, player health, and death restart written; the prologue sequence, objectives, and subtitles written; checkpoints, the cutscene, and the boss to do |
| M5 | Art, audio, and cutscenes | Tripo and Blender assets replace the greybox; lighting, sound, and the two story scenes are in | Not started |
| M6 | Lab hub and upgrades | Residue is spent on upgrades that carry into the next level | Not started |

M4 is the decision point. If the greybox level is not tense and fun to play, change the design before spending on art or planning Level 2.

## Next steps

1. Build the project in UE5 and fix any compile errors.
2. Play the test dummy, a wisp, and a rift (see the README) and tune charge, heat, damage, and speed values.
3. Block out the house from the room list in [Story.md](Story.md), using the dimensions and rules in [LevelMetrics.md](LevelMetrics.md).
4. Place the prologue's actors in the house and hook them to the prologue director (table in the README). Then make the cutscene of Mom being taken.
5. Add the flashlight, then noise and entity senses ([Audio.md](Audio.md)).
6. Write the shade and the poltergeist on top of the senses.
7. Add checkpoints, then the rift demon.

Art can run in parallel from step 3: fix the art bible, then generate and clean up kitchen and living room props.
