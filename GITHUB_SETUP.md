# GitHub 协作与项目下载说明

> 项目已存在于私有仓库：`yuqingbingxian/MythARPG`。本文件不再要求新建仓库，避免沿用旧版压缩包导入说明。

## 第一次克隆到本地

```powershell
git clone https://github.com/yuqingbingxian/MythARPG.git
cd MythARPG
git lfs install
```

随后在 Windows 安装匹配版本的 UE5.8 和 Unreal C++ 构建工具，打开 `MythARPG.uproject`。**当前仓库没有编译好的引擎二进制，也没有第一章美术/地图/蓝图资产**。

## 开发新功能的标准过程

```powershell
git pull --ff-only origin main
git checkout -b feature/quest-branch-audit
# 编辑 Source/、Plugins/、Config/、Docs/ 或 Scripts/ 下相应文件
git status
git add Source Plugins Config Docs Scripts README.md
git commit -m "docs: 完善任务系统中文说明"
git push -u origin feature/quest-branch-audit
```

团队协作建议通过 Pull Request 合并到 main，避免未经审查直接覆盖公共分支。IDE 生成的解决方案与缓存不应提交。

## 文件管理原则

- **应提交**：`.uproject`、`Source/`、`Plugins/` 的源码与配置、`Config/`、脚本和文档，以及有权使用的内容资产。
- **不要提交**：`Binaries/`、`Intermediate/`、`Saved/`、`DerivedDataCache/`、私有秘钥、帐号令牌、构建缓存。
- **大型二进制资产**：`.uasset`、`.umap`、`.fbx` 等按 `.gitattributes` 使用 Git LFS；正式提交前确认团队成员已安装并初始化 LFS。
- **不要将源码打包成 ZIP 提交到仓库**：源文件要按原有目录结构进入 Git，以便查看差异、审查和回滚。
- **文档同步**：修改 Public API、业务 ID、存档字段或依赖关系时，同一 PR 修改对应中文说明与自动化检查。

GitHub Actions 当前主要执行 Python 源码结构检查，**并不等于已经在云端构建 Unreal 项目**。真正的 UBT/UHT 与 PIE 仍需有合适的 UE 编译运行环境。
