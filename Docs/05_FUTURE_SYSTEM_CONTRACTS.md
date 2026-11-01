> 历史文档（v0.1）：新战斗代码请以 Docs/06_COMBAT_V02_DESIGN.md 和 Docs/07_EDITOR_SETUP_V02.md 为准。

# 05 — All future system interfaces (design contract; NOT code in starter)

This document exists to prevent Chapter 01 from becoming a one-off hardcoded game while keeping the C++ slice minimal.

## A. Input / Movement / Camera / Animation

Input pipeline: `Enhanced Input action -> ActionRequest {ActionId, InputTimestamp, Direction, TargetId} -> ActionPolicy -> GAS -> AnimationTask -> WindowEvents -> Result`.

Action priority table should be data-driven (`ActionPolicyDefinition`): dead > cinematic lock > execution > stagger > parry > dodge > attack > locomotion. These priorities are only defaults; exceptions are explicit override rules. Input buffer is 150–250ms target, tuned from real animations. Avoid buffering unlimited commands; expire stale actions. Action states stored as tags with explicit start/end ownership.

`UMythCameraDirector` takes a desired camera mode + interest targets and blends to a `FCameraPose` (world transform, FOV, offsets, shake) by priority, safely returning control after cinematic skip or failed lock-on. Every lock-on target requires visibility, range, world-validity and death invalidation.

Animation authority: locomotion may use Motion Matching; hit windows, i-frames, cancel windows are explicit gameplay timestamps or notify states. Anim montage does not own damage formula. Each dual-actor finisher defines warp targets and abort behavior.

## B. Combat and Enemy AI

**Combat data schema**

```
AttackDefinition {
  AttackId, AbilityClass, MontageSoftRef, SectionId,
  Windup, HitWindows[], CancelWindows[], Recovery,
  Cost, Cooldown, DamageSpec, PoiseSpec, TargetingRule,
  MotionWarpSpec, FeedbackProfile
}
HitWindow { WindowId, SocketPairs[], SweepRadius, MultiHitPolicy }
ComboNode { NodeId, AttackId, InputTransitions[], Conditions[] }
DamageSpec { SourceId, TargetId, AttackInstanceGuid, DamageTypes, RawValues, ImpactDirection }
DamageResult { FinalHealthDamage, FinalPoiseDamage, WasDodged, WasBlocked, WasParried, WasFatal }
```

Do not use animation preview collisions as authoritative hits. Implement interpolated sweeps over sampled prior/current socket transforms; deduplicate per `AttackInstanceGuid + WindowId + TargetStableId`; apply via centralized ExecutionCalculation. Test at 30/60/120 FPS and under hitches.

**AI schema**

```
EnemyBrainDefinition {
  PerceptionProfile, HomeAnchor, LeashRange, PatrolRoute,
  StateTreeSoftRef, AbilityWeightRules[], AttackSlots,
  ThreatPolicy, StuckRecoveryPolicy, ResetPolicy
}
```

AI perception -> world-state context -> StateTree selects intent -> spatial query/EQS -> `TryActivateAbilities` -> receives ability completion or failure feedback -> updates context. Encounter Director can regulate simultaneous high-threat attacks, but must not force obviously artificial waiting. Enemy respawn policy separate from persistent kills.

## C. Boss phases and arenas

```
BossDefinition {
  BossId, PhaseGraph[], ArenaProfile, RecoveryPolicy,
  PermanentDefeatPolicy, CinematicMilestones
}
BossPhase {
  PhaseId, EnterConditions[], TransitionPriority,
  AbilitySetSoftRef, BehaviorProfile, ArenaActions[],
  EntrySequence, ExitCleanupActions, MaxTransitionSeconds
}
```

Transitions are guarded transactions: request phase -> mark transition pending -> finish/cancel current attack -> apply protection rule -> fire one presentation event -> enter new phase -> release lock. Timeout/skip must call the same cleanup path. Boss defeat writes a unique durable flag and resolves rewards exactly once. A boss fight stopped by reload must obey authored policy (typically reset to start of encounter).

## D. World / chapters / streaming

Chapter definitions are primary assets and must declare entry maps, starting checkpoint, unlock predicates, data registration and soft references to dependencies. Every persistent object implements:

```
IMythPersistentObject {
  GetPersistentId(): Guid;
  GetPersistencePolicy(): enum;
  CaptureState(): FInstancedStruct;
  RestoreState(FInstancedStruct);
}
```

`WorldStateSubsystem` retains per-object state across actor unload/reload, and `ChapterRegistry` retains across travel. State restore occurs after dependencies spawned but before gameplay interactions open. Editor assigns stable GUIDs, validates uniqueness in loaded cells and entire cooked content set. `World Partition Data Layers` manage activation, not the source of truth for story flags.

`CheckpointSnapshot` and `CampaignSaveSnapshot` are separate. On death restore player spawn and resources; respawn normal encounters; never recreate opened chests or permanently defeated Bosses.

## E. Quest / dialogue / cinematics

```
QuestDefinition {
  QuestId, ChapterId, PrerequisiteConditionGraph,
  Stages[], Rewards[], FailPolicy, ExclusiveQuestGroups[]
}
QuestStage {
  StageId, OnEnterActions[], ObjectiveRules[],
  CompletionRule, FailureRule, OnCompleteActions[],
  NextStageEdges[]
}
NarrativeAction { ActionId, Payload, IdempotenceKey, Precondition }
```

Condition graph uses And/Or/Not plus typed conditions (`EventCount`, `BossDefeated`, `ItemOwned`, `StoryFlag`, `LocationReached`, etc.). Avoid embedded arbitrary Blueprint logic for primary progression. Stage actions are replay-safe and carry unique idempotence keys. Branches and stage identifiers persist to saves; a schema migration maps old IDs. During cinematic skip, apply completion actions through the same path as an uninterrupted cutscene.

**Test branches:** sequential, alternative solution, optional objective, exclusive fail, reload before final action, cutscene skipped, Boss kill before quest accepted, duplicate event delivery.

## F. RPG inventory / equipment / skills / economy

Data models:

```
ItemDefinition { ItemId, Type, StackLimit, Display, Tags, UseAbilityId, SellRules }
ItemStack { ItemId, Count }
ItemInstance { InstanceGuid, ItemId, EnhancementLevel, Affixes, Durability }
EquipmentSlot { SlotId, ItemInstanceGuid }
SkillNode { SkillId, Prerequisites, Costs, GrantsAbilityOrEffect }
LootTable { Entries[], Weight, EligibilityConditions }
```

All mutations go through an `InventoryTransactionService` with preconditions and `commit` result. Purchases and crafting must atomically deduct and grant. UI is a subscriber, never modifies data arrays directly. Define soft data references, localized names, item type validation, and stable migration IDs. Work with a single authoritative inventory on the player's save slot.

## G. UI, audio, localization and accessibility

UMG/Common UI view-models observe gameplay data via delegates and snapshots. UI layers: HUD, Menu, Modal, Cinematic, Loading, Debug; each owns focus policy and input mapping. Persistent user settings separate from campaign state. Accessibility includes scalable subtitles, camera shake intensity, hold/toggle and input remap, contrast, UI navigation, and color-blind-friendly indicators. Dialogue stores localization keys, VO references and timing data independently.

Audio Director subscribes to gameplay facts and transitions between exploration/alert/Boss/music-phase states. Sound spatialization, concurrency, submixes and haptics use authored feedback profiles rather than hardcoded sound calls in abilities.

## H. Logging / testing / build / release

Every gameplay domain has typed logs and scoped debug overlays. Editor validation catches ID duplicates, missing soft assets, invalid quest references, circular dependency graphs, unreachable Boss phases, missing localization and save migration gaps. Automation tests test pure rules and state transitions, functional tests run maps, Gauntlet packages/builds and executes smoke suites. Profile packaged builds using Unreal Insights; keep P95/P99 frame-times and memory budgets. Patch content uses versioned manifests and preserves old save IDs.

## I. Domain error handling and recovery

Define non-success result codes (`InvalidState`, `MissingAsset`, `InsufficientResource`, `AlreadyCompleted`, `SchemaMismatch`, `SaveCommitFailed`). Every asynchronous operation (travel, stream, cinematic, save) has request ID, cancellation/timeout policy and a clear final state. No game progression should depend on a one-time cosmetic callback that may never arrive.
