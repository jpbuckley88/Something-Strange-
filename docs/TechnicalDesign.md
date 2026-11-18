# Technical design

One runtime C++ module, `SomethingStrange`. Single-player, so nothing is replicated. Core systems are C++; content, level scripting, and tuning are Blueprints and data assets.

## What exists now

| Area | Files (under `Source/SomethingStrange/`) | What it does |
|---|---|---|
| Shared types | `Core/SSTypes.h` | `ESSFrequency` (Material, Spectral) and `FSSBodyPartState` |
| Game mode | `Core/SSGameMode` | Sets Ned as the default pawn and the placeholder HUD |
| Spectral vision | `Spectral/SSSpectralSubsystem` | World subsystem holding whether the goggles are on, with a change event |
| | `Spectral/SSSpectralVisibilityComponent` | Makes its actor visible only while the goggles are on |
| Player | `Player/SSPlayerCharacter` | First-person character; builds its input actions and key bindings in code |
| | `Player/SSGogglesComponent` | Charge, recharge delay, minimum charge to switch on |
| Interaction | `Interaction/SSInteractable`, `SSInteractionComponent` | Interface for usable things; a trace finds the one in focus |
| Laser | `Weapons/SSLaserWeapon` | Beam trace, heat and overheat, frequency selector |
| Body parts | `Entities/SSBodyPartComponent` | Per-part health and merge state; resolves hits to parts; frequency-matched damage; merging over time |
| Test entity | `Entities/SSTestDummy` | Stand-in built from engine shapes |
| HUD | `UI/SSHUD` | Canvas-drawn crosshair, bars, and prompts |

None of this has been compiled yet: it was written without access to Unreal Engine. Expect to fix a few compile errors on first build.

## How a laser hit is resolved

1. `ASSLaserWeapon` traces from the player's view each frame while the beam is on.
2. If the hit actor has a `USSBodyPartComponent`, the laser calls `ApplyBeamDamage` with the hit, the current frequency, and that frame's damage.
3. The component finds the part that was hit:
   - by **component tag**, where each part is its own primitive component (used by the test dummy); or
   - by **bone**, walking up the skeleton from the hit bone until one belongs to a part (for skeletal meshes, which need a physics asset for bone hits).
4. Damage is multiplied by `MatchedDamageMultiplier` if the beam frequency equals the part's matching frequency, otherwise by `MismatchedDamageMultiplier`.
5. At zero health the part is destroyed and `OnPartDestroyed` fires. If it was the core, `OnBanished` fires and the actor is destroyed.

A part's matching frequency comes from its merge amount: 0.5 or above is merged (Material), below is unmerged (Spectral). Parts with a merge rate drift towards 1, and `OnPartMergeChanged` fires when one crosses over.

## Differences from the plan

- **Frequencies are an enum, not data assets.** The project can then be tested with no content assets. If frequencies later need per-type data (icons, sounds, effects), move that into a data asset keyed by the enum.
- **Damage resolution lives in `USSBodyPartComponent`** instead of a separate damage component. One component owns the part state and the rules that change it.

## Planned

| System | Classes | Notes |
|---|---|---|
| Game flow | `USSGameInstance`, `USSSaveGame` | Level transitions, checkpoints, persistent upgrades |
| Player | `USSHealthComponent`, `USSInventoryComponent` | |
| Entities | `USSEntityDefinition`, `ASSEntityCharacter`, `ASSEntityAIController` | Definition as a `UPrimaryDataAsset` (parts, starting merge states, merge rate, behaviour, residue drop). Behaviour in StateTree. Attacks disabled when their part is destroyed |
| Merge visuals | Master entity material | Per-part merge value blending the spectral look into the solid look; unmerged parts drawn only while the goggles are on |
| Gore | `USSGoreComponent` | Listens to `OnPartDestroyed`: hides the part's bones, shows the stump, spawns the severed piece, blood and burn effects, and persistent decals |
| Goggles look | Post-process material | Uses the custom depth stencil already enabled in `Config/DefaultEngine.ini` |
| Rifts | `ASSRift`, `ASSRiftAnchor` | Spawns entities; sealed when all anchors are destroyed |
| Story | `USSObjectiveSubsystem`, `USSDialogueSubsystem` | Objectives and subtitles from data tables; cutscenes in Sequencer |
| Upgrades | `USSUpgradeDefinition`, `USSUpgradeSubsystem` | Data assets applied to goggles and laser stats |
| UI | `USSHUDWidget` and menus | UMG, replacing `ASSHUD` |

## Conventions

- Class prefix `SS` after the Unreal type prefix: `ASSLaserWeapon`, `USSGogglesComponent`.
- Includes are relative to the module root: `#include "Weapons/SSLaserWeapon.h"`.
- Tunable values are `EditAnywhere` properties with sensible defaults, so designers adjust them in Blueprint subclasses.
- Systems talk through delegates and the spectral subsystem, not by reaching into each other.
