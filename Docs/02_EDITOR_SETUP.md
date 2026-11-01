# 02｜历史 v0.1 编辑器启动说明（中文）

> **历史文档，保留用于追溯早期版本。** 当前版本优先使用 [07：v0.2 战斗编辑器操作](07_EDITOR_SETUP_V02.md)、[README：当前工程使用方法](../README.md) 与 [17：v1.0 状态](17_FOUNDATION_V10.md)。

## 目标

v0.1 只是让玩家和敌人在空白地图中完成移动、一次攻击、闪避、Boss 三段血量变化与简单手动保存的原型。不能把下文当作 v1.0 的全部功能清单。

## 搭建顺序

1. 在 Windows 以适配 UE5.8 的工具链编译 `MythARPGEditor`；代码未在本交付环境经过 UBT/UHT 检查。
2. 在 `/Game/Core/Input` 中建立 `IA_Move`、`IA_Look`、`IA_Attack`、`IA_Dodge`、`IA_Save` 和 `IMC_Player`。
3. 建立 `BP_Player`（`AMythPlayerCharacter` 子类），配置输入资源和调试模型；建立 `BP_GameMode` 并指向该玩家角色。
4. 在测试地图放 `PlayerStart`、地面和 `NavMeshBoundsVolume`，按 P 查看寻路网格。
5. 建立 `BP_Enemy`（`AMythEnemyCharacter`）与 `BP_Boss`（`AMythBossCharacter`），设置各自 `UMythCombatantDefinition` 和稳定 BossId。
6. 在 PIE 中按序验证移动、攻击、敌人追击、Boss 转阶段、死亡、F5 保存/读档。

v0.1 的攻击曾采用简单前方 Sphere 采样；**后续版本已经引入 ComboGraph、WeaponTrace、Notify、格挡/招架与动画协调**。如今请使用最新代码与 [06 动作战斗设计](06_COMBAT_V02_DESIGN.md)。

## 历史限制

早期单槽同步存档、无锁定、无任务/背包、无 StateTree 等都属于该版本的历史事实，不代表最新候选源码的全部能力。当前仓库仍没有正式游戏资产和已验证的 UE 运行结果。
