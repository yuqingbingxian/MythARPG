# 14｜v0.7 运行时稳定性修复

> 本文总结 v0.7 的已提交源码与测试要求，不是 UE5.8 的运行报告。

## 一、遭遇战预加载

`AMythEncounterDirector` 增加 Preparing 状态，在战斗开始前异步加载全部敌人类。资源准备完毕才开始 Running，避免波次中调用阻塞式类加载。

取消或失败时清理 Streamable Handle、玩家和敌人死亡回调、ActiveEnemies 与波次状态。加载完成的**迟到回调**不应重新启动已经取消的遭遇战。

### 验证案例

1. 使用尚未加载的软敌人类启动遭遇战，观察 Idle → Preparing → Running。
2. 在 Preparing 阶段销毁导演/重置战斗，验证没有意外生成敌人。
3. 连续两个波次后使用 Insights 观察是否有明显的类加载卡顿。

## 二、UI 跨地图清理

`UMythUIRouterSubsystem` 在地图切换时移除旧世界 Widget，恢复由 Router 自己拥有的输入/暂停状态。Modal 焦点按照 UI 层优先级安排，不能让旧世界 UI 阻塞新地图。

验证：打开暂停菜单时触发切图，确认加载成功后不会保留旧焦点、旧 Widget 或错误的游戏暂停。

## 三、存档快照合法性

扩展 `UMythSaveSubsystem::ValidateSnapshot`：校验关键身份字段、目标计数和数据集合大小，避免加载明显不合法的任务/世界状态。

在 Unreal Automation 搜索 `MythARPG.Foundation.Save.SnapshotValidation`。Python 静态检查不能证明存档测试已通过。

## 四、Boss 事件收据

把一次性 Boss 信号的身份约束扩大为 ChapterId + BossId，避免跨章节出现相同 BossId 时误判重复。

## 仍需处理

旧式同步资源加载审计、地图/导航回调、Outbox、输入重绑、真实动画和 Shipping 验证。后续进展参见 [15](15_AI_STATETREE_EQS_V08.md) 与 [17](17_FOUNDATION_V10.md)。
