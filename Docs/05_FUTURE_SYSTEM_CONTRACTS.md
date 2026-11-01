# 05｜跨章节扩展约定与设计边界

> 原文形成于 v0.1。这里改为**当前版本的扩展契约**，说明新增章节或复杂机制应该怎样接入；不要把设计建议误认为全部已经通过引擎验收。

## 输入 / 动作 / 动画

标准流程是 `Enhanced Input → ActionComponent → GAS Ability → Montage / Notify → Trace / GameplayEffect`。连招选择、输入缓冲、闪避取消与硬直清理属于 ActionComponent；具体 Montage 时序、Warp 目标和处决冲击通知在动画层配置。一个动作结束必须清理定时器、命中缓存、标签与异步通知绑定。

`UMythInputBindingSubsystem` 提供 `Rebind` / `SaveBindings`；必须在 IMC 提前给可重新映射的按键设置可识别名称，并验证不同设备上的冲突。

## AI 与 Boss

`AMythEnemyAIController` 管理感知、寻路和选招意图；StateTree Task / EQS 只决定“何时攻击、向哪里移动”，**技能命中与伤害仍由 GAS 战斗层完成**。`AMythCombatDirectorSubsystem` 控制同屏敌人攻击压力。

Boss 应复用战斗基类，用 `UMythBossPhaseDefinition` 定义阶段行为；击败记录永久持久化，Boss 特殊机制可以扩展但不得复制完整伤害系统。动画中断、离开战场、死亡和地图切换都必须有清理策略。

## 世界、交互和存档

章节由 ChapterCatalog 注册，地图使用软资源引用。检查点和当前地图位置属于可恢复进度；宝箱、Boss、机关等永久状态使用稳定 ID 记录。World Partition/Data Layers 只负责加载或激活世界资产，不是剧情状态的唯一真相。

## 任务与叙事

`UMythQuestDefinition` 是可编辑的任务定义，当前任务阶段和收据属于存档。业务动作应可重复投递而不重复结算；过场跳过后通过 NarrativeCommand 确定最终状态，避免把 Sequencer 当成任务数据库。

## 道具、技能、商店

物品定义与实例分离：ItemId 对应通用配置，独立装备使用 GUID，货币、强化和背包归存档。购买、制作先检查库存与成本，再一起提交结果。技能树节点根据前置 ID 决定能否解锁，不依赖 UI 视觉布局。

## UI、音频和可访问性

UMG/ViewModel 订阅状态，不能直接修改业务数据。UI 层管理 HUD/Menu/Modal/Cinematic/Loading 的焦点。音频 Director 把战斗状态映射为 SoundMix 或 SoundClass；字幕、本地化、手柄与无障碍需要独立的内容资产与人工验证。

## 错误处理与扩展守则

任何异步流程都必须有明确 RequestId、超时/取消路径、失败结果和最终清理。新增系统先提供公共 API 和测试，再扩展 Blueprint、Data Asset、编辑器校验。禁止第一章硬引用未来章节的 Actor；稳定 ID 改名需要迁移表。

## 状态说明

仓库已经包含多数上述框架的**源码入口**，但没有真实动画、StateTree、UMG 和地图资产的集成证明，也没有通过 UBT/UHT/PIE/Shipping 验收。详见 [17](17_FOUNDATION_V10.md)。
