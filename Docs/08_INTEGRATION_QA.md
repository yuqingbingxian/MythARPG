# v0.2 集成与质量门禁

## 可确认的检查范围（本次交付）

- 文件、模块、类名及 `.generated.h` 引用的**静态结构检查**。
- Python 校验 ZIP 完整性、文件清单、配置 ID 和关键源码标记。
- 由于当前交付环境不包含 Windows Unreal Engine 5.8、UnrealBuildTool 或引擎头文件：**未执行 C++ 编译、UHT 反射生成、真实 Gameplay Ability 激活、物理命中或 PIE 回归测试**。

## 引擎内必须通过的门禁（尚未执行）

1. Development Editor 编译无 Error / Warning-as-Error。
2. 运行上述两个 Automation Test（分别验证伤害公式、连招图）。
3. 无模型回退模式全程至少进行 10 次连招重试，检查动作状态与资源正确。
4. 测试 10 次长链连招、10 次强制取消和 10 次韧性打断后，`State.Combat.Attacking`、`State.Movement.Dodging`、`State.Defense.Invulnerable` 标签均能正确清理。
5. 真实 Montage 打断和复用下，不允许旧通知重新关闭当前动作命中窗口。当前需新增 Token 防护并纳入测试，见已知风险。
6. 验证一刀一目标一窗口仅结算一次，且对不同敌人均有独立结算。
7. 覆盖低帧率/高播放速率；带动画跨帧位移仍需实测高速扫掠是否漏判。
8. 锁定时走到视线遮挡后能够安全退出，切换目标时不会引用已销毁对象。
9. PI E→退出→重启，Boss 永久死亡进度正确，数据版本升级不损坏旧存档（当前存档尚不满足完整生产标准）。
10. Shipping 打包后重新检查上述流程。

## 开发状态（谨慎声明）

| 功能 | C++ 源码状态 | UE编译 / PIE 验证 |
|---|---|---|
| 连招图、输入缓冲 | 已编写 | 未验证 |
| 攻击取消、精力消耗 | 已编写 | 未验证 |
| 动画 Hit/Combo/Cancel/IFrame NotifyStates | 已编写 | 未验证 |
| 武器 Socket Sweep 命中去重 | 已编写 | 未验证 |
| 韧性与打断、基础恢复 | 已编写 | 未验证 |
| 目标锁定与镜头追踪 | 已编写 | 未验证 |
| StateTree 行为 | 未实现 | 未验证 |
| 任务与章节完整运行 | 未实现 | 未验证 |
| 商业存档兼容与安全写入 | 未实现 | 未验证 |
| 完整第一章内容 | 未实现 | 未验证 |

## 当前已知风险

- 与 Montage 相关的异步通知可能跨动画切换。当前采用互斥 ActionState 和主动关闭窗口，但仍需要 `ActionToken` / Montage 来源校验以达到商业级稳定性。
- 当前伤害使用一套定值+防御缩放，仅是验证管线；元素、防御、硬直权重、霸体、格挡/招架尚未完成。
- 目标锁定基于 Pawn Overlap + Visibility LOS，未包含巨型 Boss 特殊屏幕锚点和复杂遮挡恢复。
- Poise 受击时未包含专用 Stagger 动画及碰撞形态变化。
- 部分资源和动作参数直接使用 UObject 引用，未来需审计软引用与流送路径。
- 精力原型通过 ASC SetNumericAttributeBase 修改，尚未切入完整 GameplayEffect Cost 可预测执行器。
- 闪避位移为 LaunchCharacter，而不是 Motion Warping/Root Motion 的商业动作表现。
- 旧版同步存档是单槽实验实现，无崩溃安全及回退。
- 仅供纯单机；不存在网络预测、RPC、安全验证等多人需求。
