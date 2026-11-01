# 13｜v0.6 通用遭遇战与波次导演

> 与第一章内容无关的通用模块。v0.6 引入源码；v0.7 后改为异步预加载。未进行 UE 运行验证。

## 遭遇战导演与普通敌人 AI 的区别

单个敌人 AI 决定“如何追击和攻击”；`AMythEncounterDirector` 决定“这一组敌人按什么波次出现、何时完成、失败如何重置、奖励能否再次领取”。

核心类：

- `UMythEncounterDefinition`：Primary Data Asset，保存 EncounterId、ChapterId、Waves、敌人软类和完成奖励。
- `AMythEncounterDirector`：关卡 Actor，负责激活、生成、死亡统计、结束和重置。
- `SpawnPoints`：地图中摆放的 TargetPoint，必须与波次数据对应。

## 在 Unreal 编辑器如何使用

1. 创建 EncounterDefinition 数据资产，设置稳定 ID 和若干 Wave。
2. 每波填写敌人类引用、数量与出生点索引。
3. 测试地图放置 EncounterDirector 和一组 TargetPoint，并指定 Definition 与 SpawnPoints。
4. 玩家进入战场触发 `BeginEncounter(Player)`，战斗界面订阅 `OnEncounterStateChanged`。
5. 玩家失败或关卡重置时调用 `ResetEncounter()`。
6. 完成最后一波写入世界完成标志，并通过唯一收据发放奖励；重进场景不得重复结算。

## 后续版本修复

v0.6 初版在战斗开始时同步加载敌人软类，可能导致明显卡顿；v0.7 使用 Preparing 状态提前异步加载全部波次，并在 Reset/EndPlay 时取消句柄、委托和旧回调。**设计文档中描述“同步加载”是历史事实，不代表最新实现仍然如此。**

## 必须实际验证

预加载中死亡、重复进入、断线式销毁（单机切图）、多波连续生成、坏 SpawnPoint、敌人死亡时动画残留、清场重开、奖励只触发一次、NavMesh 和实际性能。没有真实地图/敌人资产时无法完成这些检查。
