# v0.4: campaign reliability and content auditing

This iteration changes **shared runtime foundations**, not Chapter01 content.
No `.umap`, `.uasset`, art, or executable builds are included.

## A. Two-generation PC save storage

The original v0.3 single `Saved/SaveGames/Campaign_00.sav` could be damaged
by interrupted writes. v0.4 introduces `FMythSaveStorage` (a non-UObject
storage adapter) that alternates:

- `Saved/SaveGames/Campaign_00_A.mythsav`
- `Saved/SaveGames/Campaign_00_B.mythsav`

Each file stores magic, envelope format version, increasing generation,
payload length, and CRC32. The payload is UE's normal `SaveGameToMemory`
serialization, not custom UObject serialization. The store selects the
highest *checksummed* valid generation; if one is corrupt, the older one can
still load. A new generation only replaces the inactive file. The on-disk
legacy `.sav` file is never deleted by migration. Two corrupt modern copies
are treated as corrupt and do not silently roll back to an arbitrarily old
legacy file. The write path verifies its temporary file before moving it.

This is **PC filesystem storage**, NOT yet a console-save-provider abstraction
or a fully platform-atomic rename. A sudden power loss is expected to leave at
least one valid earlier generation, but that guarantee still requires native
filesystem failure-injection testing. CRC detects accidental corruption, not
malicious tampering. Native backup/cloud-save conflict resolution is pending.

`UMythSaveSubsystem` serializes UObject state on the game thread and permits
only filesystem byte writes on the worker. A mutation after a save snapshot
was taken remains dirty; no background thread accesses UObjects.

### Save test contracts

`MythARPG.Foundation.Save.TwoGenerationsAndRecovery` tests envelope CRC,
generation selection, latest-generation corruption and successful recovery.

**Compatibility:** v0.2/v0.3 saves are loaded from `Campaign_00.sav` and
migrated to the new files at the next save point. Snapshot schema stays v2;
new disk envelope version starts at 1. Do not remove v1 schema migration fields.

## B. Travel coordinator

`UMythChapterTravelSubsystem` owns:

`Idle -> Resolving -> Saving -> OpeningMap -> Arrived` or `Failed`

It checks unlock status and active writes, requests the soft world asset,
commits chapter/checkpoint progress through the save system, opens the level,
and listens for `PostLoadMapWithWorld`. Duplicate transitions are rejected.

- `RequestChapterTravel(ChapterId, CheckpointId)` from game logic.
- `OnStateChanged` for loading-screen/telemetry hooks.
- `CancelPendingTravel()` for explicit manual recovery.

**Limits:** the travel mechanism is a hard map transition, not in-world
World Partition streaming; it does not yet implement timeouts, progress
percentage, startup-map recovery from an OS crash mid-travel, or network travel.
The first map's BeginPlay can see the selected chapter because we persist
before `OpenLevel`. Unexpected map callback attempts rollback; an OS crash or
engine map-travel failure without callback needs recovery on next launch.
No chapter map assets are created by this version.

## C. Editor-only audits

`MythEditorTools` module never enters shipping gameplay. It exposes
`UMythContentAuditLibrary::AuditAllGameAssets` to Editor Utility Blueprints,
and `AuditWorldPersistentIds` for an open level. Content checks include:

- Invalid or duplicate chapter/item IDs inside catalogs
- Missing map references or invalid item stacking limits
- Quest graph validation (unique stage and objective IDs, missing links)
- Skill prerequisite graph validation
- Dialogue start/choice references and duplicated dialogue node IDs
- Missing or duplicate persistent actor IDs within loaded worlds

Validation only scans /Game assets with the exact known Data Asset types,
so custom subclasses or external plugin paths need a future audit extension.
The loaded-world audit only checks the current world, not all chapter maps.

## D. Validation status

The Python checks (`Scripts/verify_structure.py`, `verify_systems.py`, and
`verify_foundation_v04.py`) validate source structure and integration
references. They are NOT UnrealBuildTool, UnrealHeaderTool, editor runtime or
real save recovery tests. Build and runtime integration remain unverified.

## E. Next work (still foundation, not Chapter01)

1. Save slot selection, migration test fixtures and startup recovery screen.
2. Input and keybind settings, UI routing and HUD view-models.
3. Formal StateTree/AI Perception/EQS adapters and encounter lifetime.
4. GAS advanced defense, hit-reaction and animation synchronization.
5. Dialogue actions, quest branching, conditional world persistence.
6. Automation test maps, platform packaging and runtime profiling.
