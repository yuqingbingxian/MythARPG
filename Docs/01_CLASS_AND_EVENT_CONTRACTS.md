# 01｜核心类、调用入口与事件约定

> 以仓库现有源码类名为准；早期 v0.1 的部分接口描述已经过时。未执行引擎编译。

## 一、基础设施

| 类 | 职责 | 主要公开接口 |
|---|---|---|
| `UMythSaveSubsystem` | 维护游戏进度快照和保存过程 | `GetSnapshot`、`SetSnapshot`、`ReloadFromDisk`、`RequestAsyncSave`、`FlushToDisk` |
| `UMythChapterSubsystem` | 章节登记、查询、解锁 | 与 ChapterCatalog 关联 |
| `UMythChapterTravelSubsystem` | 请求章节切图和恢复 | `RequestChapterTravel`、`CancelPendingTravel` |
| `UMythWorldStateSubsystem` | 世界永久/临时标志 | 对外状态查询和更新 |
| `UMythEventSubsystem` | 跨系统事件广播与信号投递 | 发送/消费相关业务信号 |

`UMythSaveSubsystem::SetSnapshot` 是跨域提交的底层接口，不建议普通 UI 或 Blueprint 为了方便随意直接调用；优先从任务/背包/剧情业务 API 进入。

## 二、战斗与 AI

| 类 | 负责 | 不应该负责 |
|---|---|---|
| `AMythCombatCharacter` | GAS、共同战斗状态、角色生命周期 | 永久保存全部任务 |
| `AMythPlayerCharacter` | 输入、镜头和玩家操作入口 | 道具交易数据 |
| `UMythActionComponent` | 连招、动作互斥、取消窗口和清理 | 全局剧情 |
| `UMythWeaponTraceComponent` | 命中窗口 Sweeps 与去重 | 直接修改存档 |
| `UMythDefenseComponent` | 格挡、招架和防御状态 | 选择 Boss 剧情 |
| `UMythExecutionComponent` | 双人 Montage/Motion Warping 处决协调 | 任务推进的唯一真相 |
| `AMythEnemyAIController` | 感知、移动、请求战斗决策 | 重写 GAS 攻击规则 |
| `AMythBossCharacter` | Boss 阶段与击败事件 | 单独发明另一套属性 |
| `AMythEncounterDirector` | 波次加载、敌人生成、清场和重置 | 维护全部章节存档 |

## 三、内容与用户体验

| 类 | 职责 | 常见入口 |
|---|---|---|
| `UMythBootstrapSubsystem` | 装配章节、物品、任务、成长目录 | `InitializeCatalogs` |
| `UMythInventorySubsystem` | 物品、装备、货币与制作 | `AddItem`、`RemoveItem`、`Equip`、`Craft`、`GrantOnce` |
| `UMythQuestSubsystem` | 任务开始与事件推进 | `StartQuest`、`ReportGameplayEvent` |
| `UMythNarrativeSubsystem` | 对话运行时 | `StartDialogue`、`Choose`、`StopDialogue` |
| `UMythCheckpointSubsystem` | 激活检查点 | `ActivateCheckpoint` |
| `UMythUIRouterSubsystem` | UI 层级/输入焦点/切图清理 | Widget 路由层 |
| `UMythInputBindingSubsystem` | 玩家键位重绑定 | `RegisterMappableContext`、`Rebind`、`SaveBindings` |
| `UMythAudioDirectorSubsystem` | 声音层级混音 | SoundMix/SoundClass |
| `UMythCinematicSubsystem` | Sequencer 播放与跳过 | 演出启动/结束回调 |

## 四、接口交互规范

1. **命令**：有成功/失败结果的操作，如 `TryEquip`、`StartQuest`，走公开 API 并返回明确状态。
2. **局部通知**：同一 Actor 或系统内部，采用 `Delegate`（如属性变化、死亡）。
3. **跨域业务事实**：如“某 Boss 永久击败”通过稳定 ID、一次性收据和事件层传播。
4. **可恢复流程**：存档、切图、动画、异步加载都要有失败/取消处理；避免只在“正常结束”时释放资源。
5. **资产标识**：ChapterId / QuestId / BossId / ItemId / PersistentId 发布后应稳定，重命名需要迁移方案。

## 五、入门调用示例（已有接口）

```cpp
// 从一个持有 GameInstance 的 UObject / Actor 中取得服务。
// 示例仅演示调用关系；实际需检查 GI 是否有效、资源是否已注册。
UMythQuestSubsystem* Quests = GetGameInstance()->GetSubsystem<UMythQuestSubsystem>();
if (Quests)
{
    Quests->StartQuest(TEXT("CH01_Q_Main_01"));
    Quests->ReportGameplayEvent(TEXT("BossDefeated"), TEXT("CH01_Boss_01"), 1,
                                TEXT("CH01_Boss_01_FirstClear"));
}
```

这是**用法示意，不是无需验证就可以运行的完整代码片段**。实际 Boss 完成事件应使用项目自己的持久化事件链路，避免从多个位置手动重复报告。

## 六、运行时初始化建议

启动 → GameInstance 各 Subsystem 初始化 → Bootstrap 注册 Catalog → 加载合法存档 → 解析章节和地图 → 创建 GameMode/Player/AI → 恢复世界与任务 → 挂接 UI/音频 → 解除输入锁定。

实际初始化顺序和事件订阅必须在 UE 中验证，尤其不能假设所有 GameInstanceSubsystem 的初始化顺序恰好等于头文件包含顺序。
