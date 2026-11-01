# MythARPG v0.7 — runtime reliability pass (source-only)

## Implemented

- **Async encounter class preload:** encounter enters `Preparing`, requests all wave enemy classes in one streamable load, and starts combat only after all classes are resident. No wave performs a blocking `LoadSynchronous` for enemy classes. A held streamable handle keeps classes resident until the encounter ends.
- **Cancellation and failed-wave cleanup:** reset/death/EndPlay clears pending loads, player and enemy delegates, active spawns, and wave index. Late callbacks cannot re-enter cancelled encounters.
- **UI map lifecycle:** game-instance UI router removes old-world widgets and focus during map transitions, releases only pause it owns, and resolves modal focus by layer precedence rather than insertion order.
- **Save validation:** rejecting malformed objective counts, null identity keys and absurd collection sizes before in-memory state replacement or disk serialization. A new Unreal automation source test covers sample invalid states.
- **Boss signal namespace:** once-only boss signal receipt is now scoped by chapter plus Boss ID (rather than Boss ID alone).

## Still NOT verified

- This environment cannot run UnrealHeaderTool, UnrealBuildTool, editor PIE or a packaged build.
- Asynchronous completion timing, AI possession during spawn, GC references and world-travel focus behavior MUST be tested in the editor.
- No Chapter01 content was created.

## Verification steps for the Unreal integration stage

1. Start an encounter with an unloaded enemy soft class. Confirm `Idle → Preparing → Running`; record hitch metrics.
2. Reset or kill the player while `Preparing`. Confirm no spawn and no late completion transition.
3. Advance across two waves; verify the second wave no longer invokes blocking load.
4. Open and close UI across a hard map change; verify pause state and focus restored.
5. Run `MythARPG.Foundation.Save.SnapshotValidation` in Session Frontend automation.
6. Validate duplicate `BossId` in two distinct chapters yields distinct signal receipts.

## Next required commercial-grade work

- Animation montage / Motion Warping end-to-end execution, strike windows and synchronous paired finisher animations.
- Real StateTree/EQS task bindings and behavior profile editor data validation.
- Persistent narrative event outbox and failure-safe achievement/reward delivery.
- Player rebinding persistence and device-specific Common UI focus/accessibility.
- Multi-slot save profiles, crash recovery in mid-travel, platform adapters and cloud conflict policy.
- In-engine functional tests, dedicated memory/performance gates, loading QA, and shipping build pipeline.

**Status:** Source implementation and static checks, NOT production ready.
