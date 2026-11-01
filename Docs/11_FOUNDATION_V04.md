# 11｜v0.4 双代存档、章节切换和编辑器资源审计

> 历史版本说明。仅是源码实现，未在 UE5.8 引擎运行验证。

## 为什么改造存档

原来的 `Campaign_00.sav` 是单文件形式，若覆盖时断电会丢失最新有效数据。v0.4 新增 `FMythSaveStorage`，交替保存至：

```text
Saved/SaveGames/Campaign_00_A.mythsav
Saved/SaveGames/Campaign_00_B.mythsav
```

每份文件带格式标识、代际编号、有效负载长度和 CRC32。读取时选择最新且校验有效的代；最新一份损坏时尝试使用较旧一代。旧版 `.sav` 的迁移入口保留。

`UMythSaveSubsystem` 在游戏线程捕获/序列化 UObject 快照，后台只写入字节，防止异步线程直接访问 UObject。CRC 用于检测意外损坏，**不等于防篡改，也未经过断电故障注入验收**。

## 章节切换如何工作

`UMythChapterTravelSubsystem` 的状态从 `Idle` 开始，经 `Resolving → Saving → OpeningMap`，最后为 `Arrived` 或 `Failed`。外部调用 `RequestChapterTravel(ChapterId, CheckpointId)`，新章节需要提前登记与解锁。

它是跨地图硬切换协调器，不是 World Partition 流送管理器。后续版本另增加中断恢复标志，仍需引擎测试。

## 编辑器校验如何使用

`UMythContentAuditLibrary` 是 Editor-only 类，供 Editor Utility Blueprint 调用。它检查：

- 章节和物品 ID 为空、重复以及不合法的配置；
- Quest Stage、Objective 和条件跳转；
- SkillTree 依赖；
- 对话起点和跳转；
- 当前已加载关卡持久对象的 ID 是否重复。

编辑器审核的范围受已加载内容限制，**不能把单关卡通过等同于全项目通过**。

## 需要测试

在 Unreal Automation 中运行 A/B 恢复测试；损坏最近一代文件，确认能够安全回退；在章节加载过程中强制终止并重启；在场景中设置重复 PersistentId，确认 Editor Audit 报告错误。

Python 静态检查可运行 `python Scripts/verify_foundation_v04.py`，但不能替代上述测试。
