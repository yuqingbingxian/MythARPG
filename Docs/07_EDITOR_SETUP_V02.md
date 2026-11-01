# UE 5.8 编辑器验证操作（v0.2）

**本包包含完整 C++ 源代码，无编译过的 DLL、无 `.umap` 和 `.uasset`。** 请先在 Windows 安装 UE 5.8 + Visual Studio 2022 的 Unreal C++ 工作负载和适用 Windows SDK。

## A. 导入和编译

1. 解压后应直接看到 `MythARPG.uproject`、`Config`、`Source`、`Plugins` 等。勿保留双层无意义的目录嵌套。
2. 右击 `.uproject` → Generate Visual Studio project files（或在 Editor 首次打开时自动生成）。
3. Visual Studio 中以 `Development Editor / Win64 / MythARPGEditor` 进行构建。
4. 编译若报 API 变更，先核对已安装的 5.8 次版本和项目 `EngineAssociation`；不要盲目删除 `Plugins` 内模块。
5. 打开编辑器，将 `Tools > Debug > Gameplay Debugger` 和 Automation 测试窗口作为验证辅助。

## B. 创建纯程序战斗竞技场

1. `Content/Chapters/Chapter01/Maps/L_CH01_CombatSandbox`：新建关卡、地板 Cube + PlayerStart。
2. 放 `NavMeshBoundsVolume` 覆盖全竞技场，按 P 查看寻路网格。
3. 建 `BP_GameMode`（继承 `AMythGameMode`），在关卡 World Settings 指定。
4. 建 `BP_Player`（继承 `AMythPlayerCharacter`），把 BP_GameMode.DefaultPawnClass 指向它。可以仍用内置调试 Cube 验证完整代码路径。
5. 建 `BP_Enemy`（继承 `AMythEnemyCharacter`），配置 `CombatantDefinition` 并放置两个敌人。
6. 建 `BP_Boss`（继承 `AMythBossCharacter`），设置与普通敌人不同的 CombatantDefinition；每个放置的 Boss 的 `BossId` 必须唯一。

## C. Enhanced Input

创建下列 `Input Action` 及 `IMC_Player`（示例输入，可更改）：

| 资产名 | Value type | 示例按键 | 玩家 BP 对应字段 |
|---|---|---|---|
| IA_Move | Axis2D | WASD | MoveAction |
| IA_Look | Axis2D | Mouse X/Y | LookAction |
| IA_Attack | Bool | 左键 | AttackAction |
| IA_Heavy | Bool | 右键 | HeavyAttackAction |
| IA_Dodge | Bool | Space | DodgeAction |
| IA_LockOn | Bool | 鼠标中键 | LockOnAction |
| IA_TargetLeft | Bool | Q | SwitchTargetLeftAction |
| IA_TargetRight | Bool | E | SwitchTargetRightAction |
| IA_Save | Bool | F5 | SaveAction |

WASD 要正确设置 Swizzle 与 Negate，使 IA_Move 的 `X=左右、Y=前后`。鼠标 X/Y 设置同理。把 `IMC_Player` 指定给 `BP_Player.DefaultInputContext`。

## D. 没有动画资产的检查

**无需创建 ComboGraph Data Asset。** ActionComponent 内置三段轻击和一段重击；它们使用按秒计时的命中窗口。测试先用这一模式，确认以下行为：

1. 攻击一次：0.14 秒后进入一次命中窗口，0.70 秒后恢复 Idle。
2. 在第一招 0.3～0.58 秒窗口内再次轻击，应切换到 `Light2`；第二招可轻击至 `Light3` 或重击派生至 `Heavy1`。
3. 在可取消窗口闪避，会取消攻击并释放闪避能力；窗口外闪避请求应被拒绝。
4. 连续闪避消耗精力、耗尽后拒绝动作；静止后精力恢复。
5. 持续攻击敌人令 Poise 归零后应打断技能，触发短暂硬直。
6. 对着敌人开启锁定；切换左右目标，超出范围或敌人死亡后自动取消。
7. 用 `OnPhaseEntered` (Boss Blueprint) 检查 <66% 和 <33% 的阶段转换；Boss 击败后写入存档。

**无模型时的武器虚拟点是测试回退**，碰撞范围不等于真实武器的攻击范围。真实动画接入后要禁用回退模式。

## E. 有动画以后

1. 创建 `DA_Combo_Player` 类型 `MythComboGraph`，设置 Nodes/Links；填入对应动画 Montage。
2. 打开每个 Montage，在 Notifies 轨道上按实际动作帧设置 `Myth Hit Window`、`Myth Combo Window`、`Myth Dodge Cancel Window`。
3. 把对应节点的 `bUseAnimationNotifies` 打开；保证 `Duration` 兜底时间覆盖实际播放。
4. 在使用的 Skeletal Mesh 或武器骨骼上添加 `weapon_base`、`weapon_mid`、`weapon_tip` 三个 Socket；确认扫掠位置和效果符合预期。
5. 如果要让闪避无敌帧由动画控制，设置 ActionComponent.DodgeMontage，打开 `bDodgeUseAnimationNotifies`，在闪避 Montage 上放 `Myth Dodge IFrames` Notify State。
6. 使用 Blueprint Debug、`showdebug abilitysystem` 和 Unreal Insights 验证状态，没有重复扣血或未清理的状态标签。

## F. 自动化和回归

Automation Editor 中搜索：

- `MythARPG.Combat.DamageFormula`
- `MythARPG.Combat.ComboGraph.Transitions`

请特别手动测试：动画即将完成时受击、进入 Boss 第二阶段时死亡、连击中闪避、同时按攻击与闪避、没有足够精力的闪避取消、目标死亡的瞬间切换目标、攻击后立即保存重启。

### 尚未完成（不是编辑器配置问题）

真正的动画资源、可视化调试插件、招架与盾防、多段伤害、可调相机碰撞策略、StateTree/EQS、Boss 阶段特殊能力池、持久地图对象、章节任务与剧情、完整 UI、商业级存档验证和自动化打包均属于后续开发范围。
