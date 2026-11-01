# v0.6: reusable combat encounters

- `UMythEncounterDefinition` is a Primary Data Asset with stable encounter ID, optional chapter ID, waves, enemy classes, counts and optional clear reward.
- `AMythEncounterDirector` starts encounters explicitly, spawns each wave from authored spawn points, tracks character death delegates, launches subsequent waves, and commits completion world state/rewards as one snapshot transaction.
- Player death resets and clears remaining spawns. Permanent clears persist through `Encounter.Cleared.<ChapterId>.<EncounterId>` world value.
- Data audits validate wave identities, spawn references and allowed counts.
- Added a source automation test for invalid definition cases.

## Engine integration checklist (not executed)

1. Create an Encounter Definition asset, assign enemy Blueprint classes and waves.
2. Place `AMythEncounterDirector` (or derived Blueprint) and spawn points in a sandbox map.
3. Call `BeginEncounter(Player)` from the combat-area trigger.
4. Kill every wave and verify next-wave selection, final reward receipt and world-state persistence.
5. Die mid-wave and verify no duplicate enemies or uncleared encounter reward.
6. Reopen map and verify permanently cleared encounters do not restart.
7. Profile synchronous soft-class loading; preload classes for real encounters before triggering combat.

## Known limitations

- The spawn path synchronously loads soft enemy classes. Replace with async preload for polished release transitions.
- Death animations and ragdolls remain Blueprint-owned. Dead enemies are given a five-second lifespan by the director.
- Encounter actor state is not serialized mid-wave, by design: resets restart the entire encounter.
- Arena boundaries, camera lock, intro cinematic skip, encounter music and wave UI are integration hooks, not yet authored.
- No UnrealBuildTool, UHT, PIE or packaged game verification performed.
