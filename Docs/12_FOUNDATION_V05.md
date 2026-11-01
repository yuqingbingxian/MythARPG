# 12｜v0.5 通用系统：防御、感知 AI、叙事命令、UI 与设置

> 历史版本记录。本版有对应 C++ 源码，但没有任何已执行的 UE 编译/PIE 结果。

## 一、格挡与招架：为什么从角色中分离出来？

`UMythDefenseComponent` 集中处理防御资格和伤害规则：正面格挡消耗精力、减少生命/韧性损失；招架有短暂有效窗口，成功后允许中断攻击者；精力不足导致格挡失败。这样普通敌人和 Boss 可以共用防御逻辑，而不必把它复制进每一个角色蓝图。

玩家输入中的 `GuardAction` 与 `ParryAction` 分别触发持续防御与瞬时招架。闪避、死亡、硬直时应清理防御状态。真实战斗还需要动画反馈、受击、霸体及多段命中场景的集成测试。

## 二、AI 感知与攻击规则

`AMythEnemyAIController` 通过 AI Perception 追踪目标，并用 `UMythAICombatProfile` 配置追击距离、目标遗忘、攻击距离、冷却与权重。`UMythCombatDirectorSubsystem` 为多个敌人协调同时进攻的压力。AI 负责“选择动作”，攻击和伤害仍由 GAS 执行。

v0.5 尚无完整 StateTree；后续 v0.8 已增加 StateTree/EQS 适配与原生任务，见 [15](15_AI_STATETREE_EQS_V08.md)。

## 三、可分支任务与剧情事务

任务阶段加入按条件选择的下一阶段，`UMythNarrativeCommandDefinition` 把改变世界标志、发放货币/奖励、章节解锁等后果表达为有序命令。

`UMythNarrativeCommandSubsystem` 应将多个变更放进同一份快照再提交，避免“奖励已给但任务未完成”。通过稳定收据防重复。但 v0.5 的提交后广播不是持久消息队列，崩溃可能导致业务事件没送达；v1.0 增加 Outbox，见 [17](17_FOUNDATION_V10.md)。

## 四、UI 与游戏偏好

- `UMythUIRouterSubsystem`：将 Widget 分成 HUD/Menu/Modal/Cinematic 等层次，管理输入模式、焦点和自己申请的暂停。
- `UMythHUDViewModel`：观察角色生命、精力和韧性，给蓝图 HUD 提供数据。
- `UMythSettingsSubsystem`：保存字幕、镜头、各类音量与画质设置，并与游戏存档分离。

这些只是**功能底座**。实际界面仍要在编辑器创建 Widget Blueprint，声音要创建 SoundMix/SoundClass 资产，键位重绑要配置 Player Mappable 映射。

## 五、验证和风险

新增任务分支、攻击选择、剧情指令等静态/自动化测试源码；尚无运行日志。必须测试同一输入重复触发、NPC 选项改变任务时退出游戏、UI 切换地图后焦点恢复，以及已发奖励的事务重放。
