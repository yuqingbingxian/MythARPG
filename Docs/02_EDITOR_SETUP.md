> 历史文档（v0.1）：新战斗代码请以 Docs/06_COMBAT_V02_DESIGN.md 和 Docs/07_EDITOR_SETUP_V02.md 为准。

# 02 — Editor setup and smoke-test

This is a **C++ starter**, not a precompiled, playable game. You must create native Unreal `.uasset` resources in Editor.

## 1. Compile

Open `MythARPG.uproject` with UE 5.8; build Development Editor using installed matching MSVC. If building from Visual Studio, generate project files and build `MythARPGEditor Win64 Development`. The source has not undergone UnrealHeaderTool/UnrealBuildTool verification in this environment.

## 2. Content path

Create folders: `/Game/Core/Input`, `/Game/Core/Blueprints`, `/Game/Chapters/Chapter01/Maps`, `/Game/Chapters/Chapter01/Characters`, `/Game/Chapters/Chapter01/Data`.

Create map `L_CH01_CombatSandbox`, ground, 1 PlayerStart and large NavMeshBoundsVolume covering arena. Press `P` to ensure NavMesh is green. Place 2 enemies, one boss with unique `BossId`.

## 3. Enhanced Input assets

- `IA_Move`: Axis2D, W=(0,+1), S=(0,-1), A=(-1,0), D=(+1,0) using the correct UE mapping modifiers (Negate / Swizzle) in the IMC.
- `IA_Look`: Axis2D mouse XY. If using separate mouse axes, use modifiers to map to X and Y.
- `IA_Attack`: Bool left click; `IA_Dodge`: Bool Space; `IA_Save`: Bool F5.
- `IMC_Player`: mapping context containing all the above.
- `BP_Player` (inherits `AMythPlayerCharacter`): assign `DefaultInputContext`, `MoveAction`, `LookAction`, `AttackAction`, `DodgeAction`, `SaveAction` in Class Defaults.
- `BP_GameMode`: inherits `AMythGameMode`, override DefaultPawnClass to `BP_Player`. Set in map World Settings.

## 4. Enemies

Create `DA_Enemy` with class `MythCombatantDefinition` (e.g. HP 90, damage 12, Defense 5, Aggro 1100, Attack 190). Create `BP_Enemy` from `AMythEnemyCharacter`, assign its `CombatantDefinition` and place it.

Create `DA_Boss` with HP 300, damage 25, Defense 15, ranges as desired. Create `BP_Boss` from `AMythBossCharacter`, assign CombatantDefinition, place once, set actor instance BossId `CH01_Boss_01`. Phase 1/2/3 will change by health ratio; `OnPhaseEntered` is a Blueprint event where you can log/play placeholder feedback.

## 5. Smoke checks

1. PIE: WASD movement, camera mouse movement, left mouse attack, Space dodge.
2. Enemy approaches within ~1100 cm; attacks when within its configured range.
3. Attack enemy; damage reduces HP via GAS GameplayEffect and death stops AI.
4. Boss phase event fires at <=66% and <=33% HP (with >0 health).
5. Boss defeat writes boss ID; restart game and boss should despawn on BeginPlay.
6. Move player elsewhere, press F5; restart game and confirm position/HP load.
7. In Automation window run `MythARPG.Combat.DamageFormula`.

## 6. Known prototype caveats

- Melee hits instantly at the start of the ability and in a *forward fixed sphere*. Animation hit windows are a subsequent milestone.
- Dodge launch velocity changes movement; not a fully authored root-motion dodge.
- No lock-on, hit reactions, respawned enemies, inventory, quest or UI yet.
- The saved transform only makes sense in this same map; deleting/changing the arena can place the pawn in invalid geometry.
- NPC line of sight is not considered; a target in range behind walls may trigger pursuit.
- This source package has not been built with UBT; use your actual Editor to verify and resolve patch-level API changes.
