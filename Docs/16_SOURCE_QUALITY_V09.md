# 16｜v0.9 代码质量修复：避免重复声明

> v0.9 不是新增一套玩法，而是解决之前累积的源码一致性问题。

## 曾经发现的问题

早期版本中，`AMythPlayerCharacter` 的 `GuardAction`、`ParryAction` 及对应输入回调有重复声明、定义和绑定；`AMythCombatCharacter` 中 `DefenseComponent` 的成员和创建调用也有重复。这些问题可能直接导致 Unreal C++ 编译失败或重复接收输入。

## 修复内容

- 合并 Guard/Parry 的属性声明与处理函数。
- 去除重复的 Enhanced Input `BindAction`。
- 清除重复头文件引入、DefenseComponent 成员及组件初始化。
- 新增保守的 Python 重复源码扫描脚本，并加入静态 CI。

## 如何运行扫描

```bash
python Scripts/verify_cpp_duplicates_v09.py
```

它只是文本/模式检查，并不真正解析 C++ 宏或运行 UnrealHeaderTool。即使通过，也仍可能存在 UHT、UBT、链接或运行时问题。

## 后续开发规则

修改 PlayerCharacter / CombatCharacter 前先搜索同名成员；不要通过复制大段类定义来扩展功能；跨模块改接口时应同时更新调用方、文档与测试。能复用 Component 和 Data Asset 的场景不应随意追加新的并行系统。
