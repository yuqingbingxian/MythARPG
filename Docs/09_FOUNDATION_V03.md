# v0.3 Shared Runtime Foundation (no chapter assets)

## Scope actually implemented

- Persistent `FMythCampaignSnapshot` with **V1 -> V2** migration, dirty state,
  synchronous flush, async snapshot writer, and one-slot backward-compatible
  player/Boss APIs.
- `UMythChapterSubsystem` resolves `UMythChapterCatalog` and changes the chapter
  state without hardcoding concrete chapter references into the runtime.
- `UMythWorldStateSubsystem` is durable and contains only primitive counters/flags.
- Inventory (`ItemCatalog`, unique GUID equipment instances, stacking, currency),
  skill progression, skill DAG validation, recipes, transactional shop purchase.
- Quest (`QuestCatalog`, ordered stages, objective counters, matching gameplay
  events, completion flag). Boss defeated events are bridged to quest objectives.
- Dialogue nodes / conditional choices / durable choice world flags.
- Interactable contract, raycast interaction component, persistent world object.
- Checkpoints and screen flow/audio orchestration services.
- Boss phase definition assets and token-limited multi-enemy combat coordination.

## Module DAG

```
MythFoundationRuntime
      ^         ^
      |         |
MythGameplayRuntime
      ^
      |
MythSystemsRuntime
      ^
      |
MythARPG (game module)
```

No chapter-specific classes, maps, enemy assets, quests or story scripts were
created in v0.3. Data catalogs are empty until designers register assets.

## Asset bootstrap

Create catalog assets in Unreal, then set their paths under the game config:

```ini
[/Script/MythSystemsRuntime.MythBootstrapSubsystem]
ChapterCatalogPath=/Game/Core/Data/DA_Chapters.DA_Chapters
ItemCatalogPath=/Game/Core/Data/DA_Items.DA_Items
QuestCatalogPath=/Game/Core/Data/DA_Quests.DA_Quests
ProgressionRulesPath=/Game/Core/Data/DA_Progression.DA_Progression
SkillTreePath=/Game/Core/Data/DA_Skills.DA_Skills
```

Alternatively call `InitializeCatalogs` at startup. Null catalogs intentionally
leave their feature disabled rather than crash. The above paths are examples,
**not assets included in this repository**.

### Runtime uses

- `UMythInventorySubsystem::AddItem` / `Craft` / `Equip` / `GrantOnce`.
- `UMythQuestSubsystem::StartQuest`, `ReportGameplayEvent`.
- `UMythNarrativeSubsystem::StartDialogue` / `Choose` / `StopDialogue`.
- `UMythCheckpointSubsystem::ActivateCheckpoint` (flushes snapshot).
- `UMythSaveSubsystem::RequestAsyncSave()` after safe checkpoints.
- Persistent Actors use unique `PersistentId`, restored via BP event.
- Boss phases configured with `UMythBossPhaseDefinition` and generic cues.

## Important boundaries / risks

1. **No UE compile, UnrealHeaderTool, PIE or packaged-game validation done.**
   This is source-level implementation and an API contract, not certified shipping code.
2. Async save writes a copied in-memory SaveGame object but does not yet implement
   disk-level backup/replace/checksum/rollback. Test concurrent updates before shipping.
3. Database-free transaction behavior is in-memory atomic; progress is durable
   only after a successful flush. A crash before flush loses staged mutations.
4. Item/shop/quest catalog lookups are synchronous `LoadSynchronous` prototypes.
   Replace with an Asset Manager prefetch/async loading contract once asset budgets
   are known. Do not call them mid-frame in heavy content loops.
5. Quests presently use ordered stages + objective counts, not arbitrary
   condition graph branching or a full reward/action command executor.
   Add reward orchestration and route all gameplay signals into a typed broker.
6. Boss phase cues are Blueprint implementable hooks; phase choreography,
   cancel windows, invulnerability and arena scene ownership still need work.
7. AI combat coordinator is implemented, but the legacy enemy controller still
   uses a timer-based distance AI, not production StateTree/Perception/EQS.
8. UI, cinematics, animation rig, input rebinding, accessibility, editor validators,
   difficulty settings, localization and shipping patch pipeline require separate
   engine integration. Foundation APIs reserve those seams; they are not finished.

## Verification

```sh
python Scripts/verify_structure.py
python Scripts/verify_systems.py
```

These tests are **static only**. Use Unreal automation tests after engine setup:
`MythARPG.Systems.*`, `MythARPG.Combat.Boss.Phases`.
