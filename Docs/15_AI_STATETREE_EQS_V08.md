# v0.8 StateTree + EQS adapter (source-only)

## Changes

- `AMythEnemyAIController` now owns a `UStateTreeAIComponent` and optional `UEnvQuery` tactical position definition.
- `bUseStateTree=true` stops the legacy .25-second policy timer; AI decisions are now authored via a StateTree asset configured in an AI Controller Blueprint.
- `EvaluateCombatStep()` exposes the existing attack-selection and director reservation policy to StateTree tasks without duplicating GAS logic.
- `RequestTacticalPosition()` executes an asynchronous single-result EQS query and tracks query ID. A stale result is ignored, and pending queries are removed on UnPossess.
- `MoveToTacticalPosition()` moves using a recent successful tactical query position.
- `bUseStateTree=false` continues to run the existing perception/timer decision logic.

## Editor setup (asset creation is a content authoring step, not committed here)

1. Enable StateTree, GameplayStateTree, and EQS plugins; verify game module loads.
2. Create a StateTree asset with AI Controller schema and assign it to the `StateTreeBrain` component in an AI Controller Blueprint.
3. Set `bUseStateTree=true`. Define task states for Observe, Chase, Reposition, Attack, Return, Reset.
4. In StateTree task Blueprint, call `EvaluateCombatStep` on `AMythEnemyAIController` only in a state where attacking is permitted; EQS task can instead call `RequestTacticalPosition` and observe `HasTacticalPosition`.
5. Create an EQS query with suitable target-distance/nav-path/visibility scoring and assign `TacticalPositionQuery`. Do not query on each Tick.
6. Test target death, query abort, possession switch, out-of-bounds leash, many enemies, and enemy AI debug replay.

## Limitations

- No authored StateTree/EQS assets, custom native StateTree evaluators, animation behavior validation or gameplay integration test were added.
- This is an adapter with functional controller methods, not proof of shipping quality. UBT/UHT/PIE not run.
- Tactical queries need rate limiting at content/policy level in addition to the `PendingQueryId` guard.
