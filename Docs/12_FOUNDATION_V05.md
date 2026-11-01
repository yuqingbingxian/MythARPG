# v0.5 shared systems implementation

This is **source code**, not a tested Unreal build. No Chapter01 content has been authored.

## Newly added

### Defense and combat
- `UMythDefenseComponent` is created on every combat character.
- Front-arc guard reduces health/poise damage and spends stamina; insufficient stamina breaks guard.
- Parry grants a short configurable response window and cancels the attacking ability when successful.
- Dodge removes guard, death and stagger clear defense state; attack is rejected while guarding.
- Player Blueprint must bind `GuardAction` (Started/Completed) and `ParryAction` (Started).
- The current parry response cancels the attacker; animation/hit reaction and finisher sync are *not implemented*.

### Enemy policy and sensory AI
- AI Perception sight drives target acquisition; target memory, return-to-home/leash, cooldowns, distance and weighted attack selection are authored in `UMythAICombatProfile`.
- `bDebugAcquirePlayerWithoutSight` retains a testing-only option (default false).
- `UMythCombatDirectorSubsystem` continues limiting simultaneous attacks.
- No StateTree/EQS orchestration in this version. The AI policy is explicitly an adapter, not a finished boss AI.

### Quest and narrative
- `FMythQuestBranch` enables ordered conditional stage transitions using persisted world values, with `NextStageId` as fallback.
- `UMythNarrativeCommandDefinition` supports world changes, currency additions, chapter unlocks, rewards, and post-commit gameplay signals.
- `UMythNarrativeCommandSubsystem` applies commands to a copy of the campaign snapshot and commits once, with a stable one-time receipt ID.
- Dialogue choices can reference an action sequence; conditions, commands and direct choice world flag are applied to the same snapshot.
- **Important:** event dispatch after commit is not a durable outbox. A crash in between can lose an emitted event. Receipts must be stable, and critical gameplay state must be written directly in the snapshot rather than depending solely on post-commit events.

### UI and preferences
- `UMythUIRouterSubsystem` owns a local widget stack for HUD/Menu/Modal/Cinematic layers, handles input mode and router-owned pause.
- `UMythHUDViewModel` observes Health/Stamina/Poise and exposes normalized values without UI polling.
- `UMythSettingsSubsystem` persists subtitle, input camera preferences and master/music/effects/dialogue levels independently of campaign progress. Scalability level is applied through `UGameUserSettings`.
- **Not yet bound to user-provided assets:** widget Blueprints, UI visual layouts, mix buses, camera settings consumers, user-defined key rebinding, platform accessibility and localization.

### Editor and tests
- Editor audits now scan narrative command sequences and AI attack profiles.
- Added source tests for AI attack choices, quest branch selection, and idempotent narrative commands.
- CI structural checks now include v0.5; these are not UHT/UnrealBuildTool tests.

## Class wiring

```text
MythGameplayRuntime
  MythCombatCharacter
    + MythDefenseComponent
    + MythActionComponent
    + MythWeaponTraceComponent
  MythEnemyAIController
    + UAIPerceptionComponent
    + MythAICombatProfile (optional Data Asset)

MythSystemsRuntime
  MythNarrativeCommandSubsystem
    -> MythSaveSubsystem / MythRewardSubsystem / MythEventSubsystem
  MythNarrativeSubsystem
    -> NarrativeCommandSubsystem (dialogue action)
  MythQuestSubsystem
    -> QuestDefinition conditional branch
  MythUIRouterSubsystem
  MythSettingsSubsystem
  MythHUDViewModel
```

## Known release blockers (not complete)

1. UE5.8 compilation, UnrealHeaderTool verification and in-editor runtime tests.
2. Montage task synchronization, Motion Warping, directional hit reactions and executions.
3. StateTree/EQS wiring, boss fight arena lifecycle, AI navigation edge cases.
4. HUD/menus in UMG, key-rebinding UX, applied sound mix, full localizations.
5. Durable gameplay-event outbox, multiple profiles/slots, cloud conflict resolution.
6. Full asset-data validation, cross-map GUID scanning and packaged build CI.
7. Long-run performance profiling, crash telemetry, accessibility regression and QA.

Do not advertise this uncompiled code as production-ready. No `.zip`, `.umap`, `.uasset` or built executables are committed.
