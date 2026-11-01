# MythARPG v0.2 — Unreal Engine 5.8 单机动作RPG C++源码

**本项目是原创大型 ARPG 的分章节扩展式工程雏形。当前交付一套用于第一章战斗切片的 C++ 源代码，而不是已编译的商业游戏。**

> 编译状态：未验证。交付环境没有 Unreal Engine、UnrealBuildTool、UnrealHeaderTool、Windows 编译工具链。必须在本地引擎编译并跑通 PIE/打包；详见 `Docs/08_INTEGRATION_QA.md`。

## 本次 v0.2 新增

- `UMythComboGraph`：节点/条件/边；支持轻击、重击和派生攻击，资产结构验证及一项 C++ 自动化测试。
- `UMythActionComponent`：动作状态仲裁、输入缓冲、连招衔接、取消窗口、定时安全退出、闪避资源消耗、无敌帧管理。
- `UMythWeaponTraceComponent`：攻击窗口中按 Weapon Socket 位置逐帧扫掠，按敌人去重；无 Skeletal Mesh 时有虚拟点调试路径。
- 四种 Notify State：`HitWindow`、`ComboWindow`、`DodgeCancelWindow`、`IFrameWindow`。
- GAS：`MeleeAbility`、`DodgeAbility` 改为 ActionComponent 驱动；Health/Poise 同走 GameplayEffect 修改。
- `UMythAttributeSet`：增加 Poise/MaxPoise；角色根据精力消耗延时恢复、根据韧性归零硬直打断并恢复。
- `UMythLockOnComponent`：目标搜索、LOS 校验、左右切换、锁定镜头及死亡自动退出。
- Player Enhanced Input：新增 HeavyAttack / LockOn / SwitchTargetLeft / SwitchTargetRight。

## 继承自 v0.1

3 Runtime Modules、基础 GameMode、第三人称角色、简单敌人追击、Boss 三阶段生命阈值、Boss 永久击败事件、单槽同步 SaveGame、章节主数据资产类型、伤害计算回归测试。

## 目录

- `Source/`：MythARPG 游戏模块与 Target。
- `Plugins/MythFoundation/`：事件系统、章节数据、基础存档。
- `Plugins/MythGameplay/`：角色、GAS、动作、命中、AI 和 Boss 基类。
- `Config/`：Gameplay Tags、Input、Engine 设置。
- `Docs/06_COMBAT_V02_DESIGN.md`：动作系统所有权和连招契约（从这里开始读）。
- `Docs/07_EDITOR_SETUP_V02.md`：最详细的 UE 编辑器配置和测试步骤。
- `Docs/08_INTEGRATION_QA.md`：实际已完成内容和还未验证的项目。
- `Scripts/verify_structure.py`：无引擎静态检查脚本（并非 Unreal 编译器）。

## 快速启动

1. 安装 UE 5.8、兼容版本的 Visual Studio C++ 开发工具。
2. 解压包含 `MythARPG.uproject` 的工程目录，生成工程文件并编译 Development Editor。
3. 创建 `L_CH01_CombatSandbox`、`BP_Player`、`BP_Enemy`、`BP_Boss`、`BP_GameMode` 和输入动作资产；步骤在 `Docs/07_EDITOR_SETUP_V02.md`。
4. 先不挂任何 Montage/ComboGraph，直接测试默认三轻一重、精力、韧性、闪避和锁定。
5. 跑自动化测试，再接入正式动画、Socket 和 Notify State；回归测试必须覆盖取消、死亡、中断、存档和目标丢失。

## 尚未实现

真实物料 / 动画、商用级进攻/防御细节、StateTree、关卡系统、完整任务/叙事/背包/技能树、商业存档、关卡流送、UI / 音频、性能基准与第一章完整可玩内容。v0.1 原有 docs 可能描述旧战斗方案，以 v0.2 的 06～08 文档为准。
