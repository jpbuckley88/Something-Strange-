# Game design

## Summary

| | |
|---|---|
| Genre | Story-driven first-person shooter, ghost hunting horror |
| Players | Single-player |
| Tone | Mature horror: frightening and gory. No audience rating to design down to |
| Structure | Linear levels with a hub (Ned's lab) between them |
| First target | Level 1, the house, as a complete vertical slice |

The story is in [Story.md](Story.md). The prologue, which contains the tutorial, is scripted in [Script.md](Script.md).

## Tone: scary and gory

The game is meant to frighten, and it shows what it does to bodies.

- **Merged parts are meat.** Whatever has crossed over is wet, raw, and wrong: exposed muscle, too many joints, bone where skin should be. The seam where a merged part meets an unmerged one is an open wound.
- **The laser cuts.** Destroying a merged part burns through it and severs it. Parts char, split, and drop, and what is left of the entity keeps coming. Destroying a spectral part tears it apart without blood, so the two frequencies feel different to use.
- **Merging is body horror.** A part crossing over is seen and heard: it forces its way through, tearing and setting.
- **The house keeps the evidence.** Blood, residue, and remains stay where they fell. Rooms Ned has fought in look like it.
- **Ned can die badly.** Each entity type has its own kill.
- **Fear first, gore second.** Gore lands harder after quiet. Dark rooms, sounds from the next room, and an empty goggle charge do most of the work; the gore is the payoff, not the wallpaper.

## The goggles

Ghosts are invisible or barely visible to the naked eye: a shimmer, a moving object, a cold breath. Switching the goggles on shifts Ned's vision to their frequency.

- Entities become fully visible, including the parts that have not crossed over.
- Hidden things appear: residue trails, sealed doors, the rift's anchors.
- The charge lasts a few seconds, then needs to recharge. The goggles will not switch on below a minimum charge.

The fear comes from the gaps: knowing something is in the room and having no charge left.

Starting values: 5 seconds of charge, 1.5 seconds before recharge begins, about 7 seconds to refill.

## The laser

Ned's invention. It fires a continuous beam and has a heat meter instead of ammunition: sustained fire overheats it and forces a cooldown.

It has a **frequency selector**, which is the game's ammunition change: same weapon, different beam. A beam does increased damage only to body parts on the same frequency.

| Frequency | Beam colour | Strong against |
|---|---|---|
| Material | Orange | Parts that have merged into our world |
| Spectral | Blue | Parts still on the spirit frequency |

Starting values: matched damage is doubled, mismatched damage is cut to a fifth; 4 seconds of continuous fire to overheat.

Later levels can add frequencies for new entity families.

## Partial merging

This is the core combat idea. Entities are still crossing over, so each one is only partly in our world.

- **Merged parts** (a clawed arm, a jaw, a leg) have fully arrived. They are visible to the naked eye, solid, and textured as flesh, bone, or hide. These are what can hurt Ned.
- **Unmerged parts** are still on the spirit frequency: invisible or a faint shimmer without the goggles, translucent and glowing with them.
- Each state has its own texture treatment, so the look of a part tells the player which frequency to use.
- Destroying a merged part removes that attack: no arm, no swipe.
- Destroying the **core**, which is always unmerged, banishes the entity.
- Entities keep merging over time. Leave one alone and more of it comes through, which makes it more dangerous and changes the frequency each part needs.

The combat loop: read the body, pick the part, switch frequency, fire, manage heat, switch again for the next part. Without the goggles the player sees a floating arm; with them, the rest of the body and where the core is.

## Entities (Level 1)

| Entity | Merge state | Behaviour | Teaches |
|---|---|---|---|
| Wisp | Barely merged, almost all spectral | Weak, fast, comes in groups | Aim and heat |
| Poltergeist | Only the hands have merged | Throws household objects | The goggles and the first frequency switch |
| Shade | Half merged | Stalks Ned; moves only when he is not looking through the goggles | Charge management and part targeting |
| Rift demon (boss) | Merges further each phase | Guards the rift in the lab | Everything; its parts change frequency mid-fight |

## Removing them

A defeated entity collapses into **residue**, which Ned collects. Each level ends by destroying the rift's anchors, visible only through the goggles, and sealing the rift.

## Progression

Residue and found components are spent at the lab workbench between levels:

- goggles: longer charge, faster recharge;
- laser: more heat capacity, faster cooling, new modes (a wide burst, a charged shot);
- new frequencies, as the story introduces them.

## Open questions

- Does Ned have health that regenerates, or pickups?
- Is the frequency switch instant, or does it take a moment (which would make mid-fight switching a risk)?
- What happened to Mom in the spirit realm, and how much of it does the player see?
