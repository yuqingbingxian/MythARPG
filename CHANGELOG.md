## v0.4 — Shared reliability & editor audits (source-only, uncompiled)
- PC dual-generation checksummed campaign saves; legacy .sav migration and corrupt-generation fallback.
- UObject serialization on game thread; worker performs file I/O, with queued revision save.
- Async soft-map chapter travel coordinator, event-driven phase transitions and rollback hook.
- Editor-only MythEditorTools with catalog, quest, dialogue, skill and persistent-ID checks.
- Save corruption automation test, static source audit and integration design in Docs/11_FOUNDATION_V04.md.
- No Chapter01 content, no game art, no ZIP archives committed.

# Changelog

## v0.3 - Shared non-chapter runtime groundwork

- Versioned campaign snapshot with inventory, equipment, world, quests, chapters,
  XP and unlocks; v1 save migration path and async write API.
- World flag and chapter registration services.
- New `MythSystems` plugin: items, recipes, shops, skill trees, XP, quests,
  dialogue, interaction, persistent world actors, checkpoints, audio/game flow.
- Boss phase data and enemy combat attack-token director.
- Definition validation tests and source structure verification.
- See `Docs/09_FOUNDATION_V03.md` and `Docs/10_SYSTEM_BACKLOG.md` for accurate
  status and remaining work. No first-chapter gameplay content is included.

## v0.2 - Combat action prototype

- Data-driven combo graph, stamina/poise, weapon trace, lock-on, debug fallback.

## v0.5 - Shared gameplay systems (source only)

- Added directional guard/parry/guard-break component and player input hooks.
- Added sight-based AI targeting and a data-driven weighted attack policy.
- Added conditional quest stage branches and receipt-based narrative actions.
- Added UI layer router, HUD observer, settings profile subsystem.
- Extended data audits and added three automation test sources.
- No chapter-specific content or compiled binaries. Integration remains unverified.

## v0.6 - Encounter lifecycle

- Data-driven enemy wave spawning, tracking and encounter completion.
- Player-death reset, permanent clear flag and reward transaction.
- Editor audit for encounter definitions and invalid-data test source.

## v0.7 — runtime reliability and async encounter loading

- Preload encounter wave classes asynchronously and cancel handles during teardown.
- Close UI on map travel and route focus by layer rather than push order.
- Expand save-state invariants and add a validation automation source test.
- Scope one-shot Boss signal receipts by chapter ID.
- Add `Scripts/verify_runtime_v07.py` (static only; UE execution remains unverified).

## v0.8 — optional StateTree AI and async EQS adapter

- StateTree AI Controller component, legacy policy fallback, Blueprint-callable combat decisions.
- EQS tactical position query and delayed-query teardown protection.
- GameplayStateTree modules/dependencies and editor integration instructions.

## v0.9 — remove duplicate input handlers; add static duplication detector

- Clean up repeated guard/parry declarations, definitions and mappings.
- Add source duplicate scanner and GitHub CI check.
