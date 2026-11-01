> 历史文档（v0.1）：新战斗代码请以 Docs/06_COMBAT_V02_DESIGN.md 和 Docs/07_EDITOR_SETUP_V02.md 为准。

# 01 — Class map, public APIs and stable event contracts

## Implemented classes (source-level)

| Class | Module | Purpose | Useful calls |
|---|---|---|---|
| `UMythSaveGame` | Foundation | V1 save payload | fields; not accessed directly by gameplay |
| `UMythSaveSubsystem` | Foundation | current slot state | `SavePlayerState`, `GetSavedPlayerState`, `IsBossDefeated`, `RecordBossDefeat` |
| `UMythEventSubsystem` | Foundation | domain fact bus | `PublishBossDefeated`, `OnBossDefeated` |
| `UMythChapterDefinition` | Foundation | Chapter `UPrimaryDataAsset` | `GetPrimaryAssetId` |
| `UMythAttributeSet` | Gameplay | GAS health, stamina, defense | attribute getters/setters |
| `UMythCombatantDefinition` | Gameplay | per-character base numbers | `CombatantId`, ranges/damage |
| `UMythDamageEffect` | Gameplay | instant Health delta GE | SetByCaller `FName("Damage")` (negative) |
| `AMythCombatCharacter` | Gameplay | player/enemy shared base | request abilities, strike, death |
| `UMythMeleeAbility` | Gameplay | one attack + recovery | GAS activation / end |
| `UMythDodgeAbility` | Gameplay | forward dash + i-frames | GAS activation / end |
| `AMythPlayerCharacter` | Gameplay | inputs, camera, simple save/respawn | `SaveAtCurrentLocation` |
| `AMythEnemyCharacter` | Gameplay | faction, attack ranges | `GetAggroRange`, `GetAttackRange` |
| `AMythEnemyAIController` | Gameplay | timer-based chase/attack | `EvaluateCombat` |
| `AMythBossCharacter` | Gameplay | phases / permanent boss result | `OnPhaseEntered`, `BossId` |
| `AMythGameMode` | Game | starts player | sets default pawn |

## Contract backlog: classes to implement next

**Combat/animation**
- `UMythCombatComponent`: attack-instance IDs, per-window hit tracking, attack target data.
- `UMythActionPolicy`: formal block/cancel matrix (Attack, Dodge, Stagger, Death).
- `UMythComboGraph`: data-only combo nodes + transition windows.
- `UMythHitWindowNotifyState`: begins/ends precise sword/socket sweep.
- `UMythPoiseComponent`: poise state, stagger thresholds and forced interruptions.
- `UMythTargetingComponent` / `UMythLockOnComponent`: queries, switches, occlusion, large targets.
- `UMythCameraDirectorComponent`: exploration/combat/lock-on/cinematic modes.

**AI/Boss**
- `UMythEnemyBrainComponent`: perception context and StateTree runtime binding.
- `UMythEncounterDirectorSubsystem`: attack pressure budget, leash/return rules.
- `UMythBossPhaseComponent`: ordered state transitions, guard against double-enter.
- `UMythBossMechanic`: subclassable mechanics; arena and stage-specific policies.

**Progression/world**
- `UItemDefinition`/`FItemStack`/`FItemInstance`, `UInventoryComponent`, `UEquipmentComponent`.
- `USkillTreeDefinition`/`USkillTreeRuntime`: dependency DAG; transaction-safe unlock.
- `UWorldStateSubsystem`, `IMythPersistentObject` and stable editor-assigned GUID.
- `UMythInteractionComponent`, `IMythInteractable`, `UCheckpointComponent`.

**Quest/narrative**
- `UQuestDefinition` (stages, conditions, enter/exit actions, reward).
- `UQuestRuntimeSubsystem` (stage transitions and replay/idempotence).
- `UNarrativeDirectorSubsystem` (story events); `UCinematicCoordinator` (Sequencer state & skip finalization).

**Save/UI/ops**
- `FMythSaveSnapshotV2`: profile+campaign+chapter world, migration schema.
- `UMythChapterRegistrySubsystem`: AssetManager soft-resolve and validated transitions.
- `UMythHUDPresenter`: subscribe to attribute and quest changes.
- `UMythDataValidator`: uniqueness, soft references, quest graph, boss phases.

## Event payload guidelines

Use ID, context, occurrence ID, optional source reference with safe lifetime, and timestamp if required.
For `BossDefeated` production payload:

```
FMythBossDefeatedEvent {
  FName BossId; FName ChapterId; FGuid OccurrenceId;
  FName SourceQuestId; bool bPermanent;
}
```

The starter's event intentionally has only two FName params. Upgrade it before multiple listeners/retries are used in the first fully playable quest. A delivery event is not the state of record; state must be explicitly committed once.

## Runtime startup contract (target)

1. `UGameInstance` initializes Foundation subsystems, validates config and SaveData version.
2. `UGameMode` creates player; character initializes ASC `InitAbilityActorInfo(Owner, Avatar)` and grants starter abilities.
3. `ChapterRegistry` loads Chapter definition by PrimaryAssetId and its entry map.
4. `WorldStateRegistry` materializes stable states after world Actors are spawned / streamed.
5. `QuestRuntime` attaches to scoped event streams and restores active stages.
6. `Player` restores checkpoint and inventory/attributes; UI receives snapshots.
7. `NarrativeDirector` selects entry cutscene or gameplay; input mode moves to GameOnly.
8. Save/Stream/Cinematic shutdown cancels timers, callbacks and pending operations, preventing double execution.

The starter only implements #1 (save initialization), #2 and selected portions of #6.
