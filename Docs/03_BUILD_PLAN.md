> 历史文档（v0.1）：新战斗代码请以 Docs/06_COMBAT_V02_DESIGN.md 和 Docs/07_EDITOR_SETUP_V02.md 为准。

# 03 — Implementation order, PR breakdown and Definition of Done

## PR-001: foundation (starter source included)

- Create modules + foundational save/event/chapter data classes.
- Build Windows Development Editor and cook a blank map.
- **DoD:** No compile warning treated as error, editor loads, test commandlets run.

## PR-002: GAS combat slice (starter source included, editor verification pending)

- Character ASC, attributes, melee and dodge abilities, Damage GameplayEffect.
- Input, camera, debug mesh, enemy AI, Boss phase/death, minimal save.
- **DoD:** Attack kills enemy, dodge avoids damage, Boss phases and permanent defeat survive restart.

## PR-003: make combat production-safe

- Implement `ActionPolicy`, animation montage abilities, AnimNotify hit windows, multi-socket temporal sweeps, duplicate-hit prevention.
- Add per-hit damage context, precision guard, stamina cost, poise, cancel priorities.
- **DoD:** deterministic 3-hit combo under varying FPS and cancellation scenarios; automated attack tests pass.

## PR-004: perception + StateTree + Boss phase transitions

- AI perception vision/hearing/aggro memory, navigation failure handling, data-driven ability choice.
- Boss phase runtime waits for safe transition, cancels/ends prior ability, triggers unique cinematic, recovers after skip.
- **DoD:** Boss can repeatedly be entered, defeated, abandoned and restored without softlocks.

## PR-005: world + interaction + checkpoint

- Build world persistable actor GUID, local region registry, streaming load/unload callbacks.
- Interactable interface; doors, pickup, chest, checkpoint, region transitions, encounter respawn policy.
- **DoD:** opened chest remains opened after unload/reload and game reboot; normal mobs respawn under policy.

## PR-006: quest + narrative and first mainline loop

- Quest stage conditions/actions, typed event registry, idempotent rewards and world flags.
- Dialogue, cinematic state machine and mandatory skip finalizer.
- **DoD:** Start -> NPC -> kill Boss -> open gate -> chapter complete, uninterrupted or after any save/reload.

## PR-007: RPG growth and UI

- Item definition vs item instance, transactional inventory operations, equipment, skill DAG, consumables and loot.
- HUD, pause, map, subtitles, remapping, settings, gamepad UI focus.
- **DoD:** defeat enemies, earn resources, grow character, reload and use resulting build.

## PR-008: Chapter 01 content and release

- Design five connected regions, two bosses, sidequests, cutscenes, dialogues, tuning.
- Async + versioned save with backup, failure handling, content validation commandlets.
- Full automated packaged run, stability soak, profile against signed-off target hardware.
- **DoD:** a new player completes Chapter 01 in a Shipping build with no blocking bugs.

## Non-negotiable engineering constraints

- No gameplay logic in Level Blueprint except temporary debug hookups.
- No persistent Actor pointers in save slots.
- No hardcoded future chapter asset paths in Chapter 01.
- All rewards, boss deaths, quest-stage side effects idempotent.
- Every new system must have at least one Debug endpoint and one automated test.
- Changes to IDs require a migration manifest and backwards-compatibility test.

## First concrete milestone acceptance

Check all of: compile, open map, move/look, fight enemy, dodge invul, AI pursuit, boss 2 transitions, boss persistence, manual player save, unit-test result. This is a vertical slice; do not confuse it with a completed commercial chapter.
