# 15｜v0.8 AI 框架：StateTree 与 EQS 如何配合

> v0.8 加入 AI Controller 适配层，v1.0 补充原生 Task。仓库没有已配置的 StateTree 或 EQS 资产，也没有在 Unreal 中运行验证。

## 一、四个系统各自解决什么问题

| 系统 | 解决的问题 | 例子 |
| --- | --- | --- |
| AI Perception | 玩家是否被看到、是否丢失目标 | 视觉感知到玩家进入警戒范围 |
| StateTree | 敌人当前该做什么 | Observe / Chase / Reposition / Attack / Return |
| EQS | 敌人应该站在哪个位置 | 寻找可达、能看到玩家、距离合适的攻击点 |
| GAS | 真正执行攻击和规则 | 激活攻击 Ability、消耗精力、应用 GameplayEffect |

**原则：决策不能重新实现战斗规则。** StateTree Task 只向既有战斗框架请求技能，避免两套扣血逻辑。

## 二、已有源码连接

`AMythEnemyAIController` 持有 StateTree 组件与可选的战术查询 `UEnvQuery`：

- `bUseStateTree=true`：使用 StateTree 模式，关闭原先的周期策略 Tick。
- `bUseStateTree=false`：保留原来感知和定时器驱动的调试回退。
- `EvaluateCombatStep()`：复用攻击选择和 CombatDirector 限额规则。
- `RequestTacticalPosition()`：异步执行 EQS 单结果查询。
- `MoveToTacticalPosition()`：依据最近成功的 EQS 位置进行导航。

v1.0 源码提供 `FMythCombatDecisionTask` 和 `FMythTacticalMoveTask`，通过 StateTree 的绑定参数获取正确 AI Controller；每次 Task 实例的数据不能使用共享的可变状态。

## 三、编辑器配置步骤

1. 启用 StateTree、GameplayStateTree、Environment Query 相关功能（具体依项目引擎版本与模块启用情况核对）。
2. 创建 AIController Blueprint，指定合适 AI Schema 的 StateTree Asset，设置 `bUseStateTree=true`。
3. 在 StateTree 配置 Observe、Chase、Attack、Reposition、Return 和 Reset 等状态，分别绑定感知、决策与导航任务。
4. 创建 EQS Query，配置可行走点、目标距离和视线评分，并绑定到 `TacticalPositionQuery`。
5. 设置触发节流：不得每帧都发新的 EQS Query。
6. 让两只敌人同时攻击，验证 CombatDirector 对高威胁攻击的并发限制。

## 四、必须处理的异常情况

目标死亡、目标进入不可达区域、切图、敌人死亡、Controller UnPossess、StateTree 退出、NavMesh 缺失以及 EQS 延迟回调。Task 退出后必须取消或忽略属于旧请求的回调。

## 五、尚未验收

StateTree 属性绑定、原生 Task 生命周期、EQS 正确寻路、异步取消和性能表现都需要在真实 UE 关卡里测试。静态检查只能确认部分源码结构。
