# Audio plan

The goggles are off most of the time, so the player's ears are how they know something is in the room. Sound is a gameplay system here, not decoration. This plan covers what has to be decided before more of the game is built; nothing in it is implemented yet.

## Principles

1. **If it can kill Ned, it can be heard.** Every entity has a sound the player can learn while it is invisible.
2. **Sound tells the truth.** A noise in the next room means something is in the next room. No fake scares from sounds with no source.
3. **Quiet is the default.** The house is near silent, so small sounds carry and the player listens.
4. **Sound works in both directions.** The player hears them; they hear the player. See [Noise and senses](#noise-and-senses).

## Entity tells

Each entity gets three sounds the player learns to read: one when it is idle and unseen, one when it has noticed Ned, and one just before it attacks.

| Entity | Idle, unseen | Noticed Ned | Before attacking |
|---|---|---|---|
| Wisp | Thin, wavering hum, like a wet finger on glass | Hum rises in pitch and steadies | Short rising whine |
| Poltergeist | Nothing of its own: objects shift, cupboards creak, something rolls | Objects rattle together | An object scraping as it lifts |
| Shade | Silent | Slow, wet breathing, only when close | Breathing stops |
| Rift demon | Low pressure you feel more than hear | Its voice: layered, wrong | Different per attack, so each can be learned |

The shade's silence is its tell: it is the one entity the player has to find with the goggles.

## Merged and unmerged

The two states sound as different as they look, so the ear confirms what the eye reads.

| | Merged | Unmerged |
|---|---|---|
| Movement | Wet, heavy, physical: flesh on floorboards | Thin, distant, slightly reversed |
| Hit by the matched beam | Searing, splitting, burning | A shriek that drops in pitch, glass-like cracking |
| Hit by the wrong beam | Dull hiss, almost nothing | Faint ringing, almost nothing |
| Destroyed | Tearing, then something hitting the floor | Implosion, then silence |

**Crossing over** gets its own sound: a tearing, like canvas, with a wet finish. It warns the player that a part has changed frequency without them looking.

## The goggles

- **Switching on:** a capacitor whine, then a click.
- **While on:** the real world drops back (lower volume, high frequencies rolled off) and the spectral layer comes forward. Unmerged parts are much louder through the goggles.
- **Running low:** the whine wavers in the last second, so the player hears the charge ending.
- **Switching off:** the real world comes back with a slight ear-pop.
- **The flashlight** has a plain click on and off.

## The laser

- **Each frequency has its own tone.** Material is low and rough; spectral is high and clean. The player can tell which is selected without the HUD.
- **Heat is pitch.** The beam's tone rises as heat builds, so an overheat is heard coming.
- **Overheat:** a cut-off, a hiss of venting, ticking metal as it cools, and a soft chime when it is ready.
- **Frequency switch:** a mechanical dial click plus a short retune.

## The house

- **Closed rooms and real doors.** Unreal muffles sound through walls and closed doors. "Something moving in the next room" depends on rooms being enclosed and doorways having doors. This is the one audio decision that shapes the blockout; see [LevelMetrics.md](LevelMetrics.md).
- **A reverb per room type.** Small and dead for bedrooms, hard for the kitchen and bathroom, boomy for the basement, boxy for the attic.
- **Room tone.** Each room has a near-silent bed: fridge hum, a clock, pipes, wind in the attic. After the accident most of these stop, and the house's own creaks take over.
- **Daylight and dark.** The same house needs two sound sets, like its two lighting sets: lived-in for the prologue, dead for Level 1.

## Surfaces

Footsteps depend on the floor, for Ned and for anything merged enough to have feet. Tag every floor with a surface type as it is blocked out.

| Surface | Where | Loudness |
|---|---|---|
| Carpet | Bedrooms, living room | Quiet |
| Wood | Hall, stairs, attic | Medium; stairs and attic creak |
| Tile | Kitchen, bathroom | Loud |
| Concrete | Basement, garage | Medium, with echo |

Loudness matters because entities hear footsteps.

## Noise and senses

Entities sense Ned and react to his sounds and his interactions. One system serves both the audio and the AI: everything Ned does that makes a sound also emits a **noise event** with a loudness, and entities within range hear it.

| Action | Loudness |
|---|---|
| Walking on carpet | Very low |
| Walking on wood or concrete | Low |
| Walking on tile, creaking stairs | Medium |
| Opening or closing a door, taking an item | Medium |
| Throwing a switch, using the workbench | Medium |
| Firing the laser | High |
| Destroying a part, an overheat venting | High |
| The goggles and the flashlight | Silent |

Closed doors between the noise and the listener cut how far it carries.

**Entity awareness** has three states:

1. **Unaware.** It does whatever it does when alone.
2. **Investigating.** It heard something and goes to where the noise came from. Its "noticed" sound plays.
3. **Hunting.** It has found Ned and attacks. If it loses him, it returns to investigating the last place it sensed him.

**Each entity senses differently**, which makes them play differently:

| Entity | Hears | Sees | Special |
|---|---|---|---|
| Wisp | Poorly | Well, at short range | Drawn to the laser: firing pulls in every wisp nearby |
| Poltergeist | Well | Does not | Reacts to interactions: anything Ned opens, takes, or switches tells it where he is |
| Shade | Very well | Well | Freezes while watched through the goggles |
| Rift demon | Always knows where Ned is | | |

This replaces the current behaviour, where every entity chases Ned from anywhere in range.

**Open questions:**

- Should the flashlight make Ned easier to see? It costs nothing to run, so being noticed sooner would give it a price. My recommendation is yes, for the shade only.
- Does Ned get a quiet walk or crouch? Without one, the only way to be quiet is to stand still.
- Can Ned hide (in a wardrobe, under a bed), or only break line of sight?

## Voice

- Every line gets an ID, so recordings match subtitles: `PRO_S1_MOM_010` is prologue, scene 1, Mom, line 10. Numbering in tens leaves room to insert lines.
- Ned needs two kinds of line: scripted dialogue, and short reactions (breathing, pain, "no no no") that play from gameplay.
- Until there is a cast, record placeholder lines yourself. Timing subtitles to real audio early shows which lines are too long.
- Mom's voice continues after she is taken: the answering machine, a voicemail on Ned's phone. Cheap, and it keeps her in the game.

## Music

Sparse. Music is information, so it is used rarely.

| State | Music |
|---|---|
| Exploring, nothing near | None. Room tone only |
| Something is aware of Ned | A low tension layer fades in |
| Hunted | Pulse and dissonance |
| Boss, story beats | Full, written pieces |
| Prologue | A warm, simple theme for home and Mom, which returns, broken, later |

## Technical choices

- **Unreal's built-in audio.** MetaSounds for anything that changes with gameplay (laser pitch with heat, entity hum with awareness). No outside middleware.
- **Sound classes** with their own volume sliders: Master, Effects, Voice, Music, Ambience.
- **A goggles mix** applied while they are on: real-world classes ducked and filtered, the spectral class raised.
- **Attenuation presets** shared across sounds: small (an item), medium (footsteps, doors), large (the laser, the demon).
- **Naming:** `SFX_<Thing>_<Action>_<Variant>`, `VO_<LineID>`, `MUS_<State>`, `AMB_<Room>`.

## What the code already provides

These events exist, so sounds can attach to them without code changes:

| Event | Sound |
|---|---|
| Goggles toggled | On, off |
| Laser overheated, recovered | Cut-off, vent, ready chime |
| Laser frequency changed | Dial click, retune |
| Part merge changed | Crossing over |
| Part damaged (with frequency and whether it matched) | Hit sounds, matched or not |
| Part destroyed, entity banished | Destruction, implosion |
| Door toggled, power switched, workbench assembled | Their own sounds |
| Dialogue line started | Voice line |

## What needs building

| Need | Notes |
|---|---|
| Noise events and entity hearing | The system in [Noise and senses](#noise-and-senses) |
| Entity sight and the three awareness states | Replaces chase-on-range |
| Surface types on floors, footsteps | Footsteps emit both the sound and the noise event |
| Laser heat and beam state exposed to audio | For pitch with heat |
| Goggle charge exposed to audio | For the low-charge waver |
| Voice line IDs on dialogue lines | With the move to a data table |
| The flashlight | See the game design |

## Sound list: prologue and Level 1

A starting list, so sourcing can begin alongside the art.

- **Ned:** footsteps on four surfaces, breathing (calm, scared, hurt), pain, death.
- **Goggles:** on, loop, low-charge waver, off, flashlight click.
- **Laser:** two beam loops, dial click, retune, overheat, vent, cooling ticks, ready chime.
- **House, daylight:** fridge, clock, pipes, distant traffic, front door, interior doors, freezer, phone.
- **House, dark:** creaks, wind, power failing, breaker, power returning.
- **Prologue props:** paint can bursting, meat searing, jar cracking, workbench assembly, aperture switches, the aperture powering up and tearing open.
- **Entities:** three tells each for wisp, poltergeist, shade; movement, hit, and destruction for merged and unmerged; crossing over.
- **Rift:** loop, anchor destroyed, sealing.
- **Voice:** every line in [Script.md](Script.md).
