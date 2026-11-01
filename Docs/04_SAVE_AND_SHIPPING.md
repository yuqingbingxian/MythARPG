# 04 — Save hardening and shipping checklist

The starter uses **synchronous, single-slot `USaveGame`** for simple testing. This is deliberately **not production-safe**.

## Upgrade plan

1. Create immutable `FSaveSnapshotV2` on game thread at safe frame boundaries. Include Profile/Campaign/ChapterWorld state, stable GUIDs, schema/build version.
2. Serialize snapshot off-thread; use temp file, flush and atomic rename when platform allows, retain last known-good backup.
3. Separate checkpoint respawn policy from persistence policy (mob reset versus permanent quest/Boss/chest flags).
4. Use stable IDs, not path names or transient pointers. Build `UWorldStateRegistry` that re-applies per-actor state after streaming.
5. Use ID migration tables + schema migration tests for patches. Test game exit during save and corrupted file fallback.
6. Ensure item/reward changes and related quest completion are transactionally consistent (exactly-once semantics).
7. Validate saved map/ChapterId before applying transform. If invalid, fall back to a safe checkpoint.
8. File slot UI, manual/auto save policy, backup recovery and platform-specific store integration.

## Test matrix

- Save while enemy dies; save during cutscene skip; save at chapter boundary; save after Boss reward.
- Crash/restart before completion and after commit.
- Load old versions of each schema generation.
- Stream out/in opened chest, defeated elite and NPC progress.
- Re-enter boss arena after victory; abort boss arena mid-transition.
- 50+ hours mock accumulated state, memory soak, packaged Shipping build.

## Performance budgets

60 fps corresponds to ~16.67 ms per frame; thread/GPU budgets must be profiled separately on actual target hardware using packaged builds and Unreal Insights. Record P50/P95/P99 frame times, load hitches, max memory, AI update count, ability count and active world actors. No performance figure is claimed for this unbuilt starter.
