# v1.0 candidate foundation source — integration still REQUIRED

**Scope:** reusable game foundation, not Chapter01 content, art or Blueprints.

Implemented in C++ source in this revision:

- Native `UMythExecutionComponent`: paired montage startup, motion-warp target,
  one authored impact notification and idempotent release on interruption/death.
- `UMythActionComponent`: montage ended callbacks in addition to timer fallback,
  including interrupted attacks and dodge cancellations.
- `FMythCombatDecisionTask` / `FMythTacticalMoveTask`: native StateTree tasks
  with bound controller and per-instance state (not shared mutable task data).
- `UMythInputBindingSubsystem`: per-local-player remappable keys using
  EnhancedInput user settings. Input Mapping Contexts must be registered with
  user settings and use Player Mappable mapping names.
- `UMythAudioDirectorSubsystem`: SoundMix/SoundClass gain routing, change
  notification and level-travel reapplication.
- `UMythCinematicSubsystem`: runtime Sequencer play/skip/end, input cleanup.
  Narrative state changes must remain separate from Sequencer tracks.
- Campaign snapshot persistent signal outbox. Story command side-effects and
  queued signals are committed together before delivery. Delivery may replay
  after crashes; consumers require receipt-based idempotency.
- Quest event consumer records stable receipts with objective progression.
- Chapter travel recovery record: a game start after interrupted OpenLevel
  restores the previous chapter/checkpoint/player transform.
- Source unit tests for outbox and chapter recovery validation.

## Required UE5.8 integration / acceptance tests (NOT PERFORMED)

1. Compile Editor, Game and Shipping targets with UnrealBuildTool/UHT.
2. Use temporary compatible skeletons for paired execution montages, add warp
   modifier windows and a named warp target. Confirm natural end, death,
   interrupt, map travel, skip/overlap behavior, unmodified movement state.
3. Create StateTree assets with bound AI controller and both native tasks,
   supply a real EQS tactical query and test no-nav, target death and query
   cancellation. Tune tick period and query timeout.
4. Create player-mappable input rows in IMC, test key conflict, keyboard vs
   controller, multiple slots, restart and profile persistence.
5. Assign SoundMix + SoundClass routing assets and a Level Sequence; validate
   pause, map transition, late loading and skip.
6. Kill app immediately after a narrative state commit and before receipt ACK,
   restart and verify exactly-once *quest progress*, no duplicate rewards;
   test crash during chapter travel and both save generations corrupt.
7. Test 60FPS target hardware in a *packaged* Shipping build with Insights,
   memory tracking and soak test. Add CI Windows runners with licensed UE.

## Known design caveats

- Persistent outbox guarantees at-least-once attempts, **not global exactly-once
  effects**. Each business consumer must record its own stable receipt. Quest
  consumer includes dedup in this revision; future subsystems require the same.
- Some pre-existing synchronous asset loads and module seams still need runtime
  profiling. No claim is made about compiler or gameplay correctness.
- Story, animation, settings and audio assets must be configured in Editor;
  code cannot invent montage windows, input mapping names or camera tracks.
- `UMythCinematicSubsystem` does NOT commit story progression. Callers respond
  to `OnCinematicFinished` and commit a separate narrative transaction.
- The completed source feature surface is a **candidate for integration**, not
  a verified commercially shippable game foundation.
