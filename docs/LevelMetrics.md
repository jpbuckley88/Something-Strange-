# Level metrics

Standard dimensions for blocking out the house, so props, doors, and enemies fit everywhere and nothing is rebuilt later. Unreal units are centimetres.

These are starting values chosen to fit the code as written. Adjust them once after walking a test room, then keep them fixed.

## Ned

| | Value | Source |
|---|---|---|
| Height | 176 | Capsule half-height 88 |
| Width | 80 | Capsule radius 40 |
| Eye height | 152 | Camera 64 above capsule centre |
| Reach | 250 | Interaction range |
| Highest step he can climb | 45 | Engine default |

## Architecture

| Element | Dimension | Why |
|---|---|---|
| Grid snap | 10 | Everything lands on the grid; walls on 50 where possible |
| Ceiling height | 270 | Basement 240, attic 200 at the ridge sloping to 100 |
| Interior wall thickness | 20 | Thick enough to block light and sound cleanly |
| Exterior wall thickness | 30 | |
| Doorway opening | 110 wide, 210 high | Ned plus clearance |
| Door leaf | 100 wide, 200 high, 10 thick | Matches the placeholder door in code |
| Corridor width | 140 minimum, 160 preferred | Room to back away from something |
| Stair width | 110 minimum | |
| Stair step | 18 rise, 28 run | A real-world stair; well under the 45 limit |
| Window sill | 90 above the floor | |
| Counter and workbench height | 90 | Matches the placeholder workbench |
| Table height | 75 | |

## Rooms

Rough interior sizes for a believable house that still has room to fight in.

| Room | Size |
|---|---|
| Hall | 160 wide, as long as needed |
| Kitchen | 400 x 450 |
| Living room | 500 x 600 |
| Bedrooms | 350 x 400 |
| Bathroom | 250 x 300 |
| Basement lab | 700 x 900: the largest space, because the boss fight is here |
| Garage | 600 x 600 |
| Attic | The footprint of the upper floor |

## Rules for the blockout

1. **Rooms are enclosed.** Four walls, a ceiling, and a door in every doorway. Sound and light both depend on it.
2. **Every room has two ways out** where the house allows, so the player is rarely cornered without having chosen it. The basement is the deliberate exception.
3. **Sight lines are short.** Corners, door frames, and furniture break up views, which keeps things close when they are seen.
4. **Leave fighting space.** A clear circle about 300 across in any room where enemies appear.
5. **Tag floors with a surface type** as they are placed: carpet, wood, tile, or concrete. See [Audio.md](Audio.md).
6. **Add a navigation mesh volume** around the whole house. Ground enemies cannot move without it.
7. **Mark hiding places and noise makers** as you go: wardrobes, beds, creaky stairs, a loose floorboard.

## Enemies

| Entity | Size | Fits through a doorway |
|---|---|---|
| Wisp | 60 across | Yes |
| Poltergeist, shade | About Ned's size | Yes |
| Rift demon | Larger than a doorway | No: it stays in the basement |

## Light

- After the accident the house has no power. Light comes from Ned's flashlight, windows (streetlight, moon), and the rift's glow.
- The flashlight is a narrow cone reaching about 1,200. It should light what Ned points at and leave the rest of the room dark.
- Build the house once and light it twice: daylight for the prologue, dark for Level 1.
