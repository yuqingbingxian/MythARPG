# 更新日志｜MythARPG

> 以下均为**源码提交范围**。除非另有独立的 UE 构建与测试报告，否则不代表引擎已编译或商业级验收完成。

## v1.0 候选版｜动画、AI、叙事可靠性

- 增加双角色 Montage + Motion Warping 处决同步及中断清理。
- 完善动作组件 Montage 生命周期回调。
- 新增原生 StateTree 决策任务与 EQS 战术移动任务。
- 新增本地玩家 Enhanced Input 键位重绑定服务。
- 增强 SoundMix/SoundClass 混音、Sequencer 播放和跳过。
- 增加持久化剧情信号 Outbox、任务消费收据与章节切图恢复标志。
- 详见 [17](Docs/17_FOUNDATION_V10.md)。

## v0.9｜源码质量

- 修复部分格挡、招架和 DefenseComponent 的重复声明与绑定。
- 增加保守的 Python 重复定义扫描（不能替代 C++ 编译）。

## v0.8｜StateTree 和 EQS 接口

- AIController 增加 StateTree 可选运行模式、异步 EQS 位置查询与旧决策回退。
- 详见 [15](Docs/15_AI_STATETREE_EQS_V08.md)。

## v0.7｜运行时清理

- 遭遇战异步预加载敌人类，取消、死亡和销毁时释放加载任务/委托。
- UI 切图时释放旧世界 Widget 焦点。
- 扩展存档合法性检查，Boss 信号按章节与 Boss ID 区分。
- 详见 [14](Docs/14_RUNTIME_V07.md)。

## v0.6｜遭遇战导演

- 数据驱动波次生成、敌人计数、重置和一次性结算。
- 详见 [13](Docs/13_ENCOUNTER_V06.md)。

## v0.5｜高级防御、叙事与 UI 入口

- 增加正面格挡、招架、AI 视觉感知与可配置攻击策略。
- 增加任务条件分支、剧情指令事务、基础 UI 路由、HUD 属性 ViewModel 与设置持久化。
- 详见 [12](Docs/12_FOUNDATION_V05.md)。

## v0.4｜可靠存档与编辑器校验

- 双代存档、CRC 校验、旧版迁移、章节地图切换状态机。
- 增加 Editor-only 配置校验。
- 详见 [11](Docs/11_FOUNDATION_V04.md)。

## v0.3｜通用游戏服务

- 游戏进度快照、章节目录、背包与装备、技能成长、任务对话、交互与检查点、商店与制作、世界状态。
- 详见 [09](Docs/09_FOUNDATION_V03.md)。

## v0.2｜动作战斗原型

- 连招图、输入缓冲、命中窗口与武器扫掠、精力、韧性、锁定目标。
- 详见 [06](Docs/06_COMBAT_V02_DESIGN.md)。

## v0.1｜最初的程序骨架

- 游戏模块、GAS 初始能力、基础敌人和 Boss、简单玩家位置/生命存档。
- 早期文档已标注历史版本。
