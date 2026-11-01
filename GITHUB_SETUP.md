# Upload MythARPG to GitHub

This archive is the full v0.2 Unreal **source prototype**, not a compiled game.
The Unreal Engine 5.8 build, UnrealHeaderTool and playable PIE tests have
NOT been run.

## 1. Create a private repository

Open https://github.com/new and choose:
- Owner: `yuqingbingxian`
- Repository name: `MythARPG`
- Visibility: **Private**
- Do **not** initialize with README, .gitignore or license
  (this source repository already has its own history).

## 2. Push from Windows PowerShell

Install Git for Windows. Unzip this archive and open PowerShell in
the `MythARPG` folder (containing `MythARPG.uproject`). Run:

```powershell
git remote add origin https://github.com/yuqingbingxian/MythARPG.git
git push -u origin main
```

If prompted, sign in with Git Credential Manager. You do NOT need to
paste a personal access token into this repository or chat.

Alternatively, once that empty repository exists and your ChatGPT GitHub
connection is granted write access to it, ask ChatGPT to upload the
current project to that repository.

## 3. Clone later

```powershell
git clone https://github.com/yuqingbingxian/MythARPG.git
cd MythARPG
```

You must still configure Unreal Blueprint assets and build the project
locally. The GitHub Actions workflow checks structural file references
ONLY, not Unreal compilation.

## Future asset policy

Git LFS is configured in `.gitattributes` for common Unreal and art
binary assets. Run `git lfs install` on every development workstation
before adding `.uasset`, `.umap` or large binary assets.

Avoid uploading Binaries/, Intermediate/, DerivedDataCache/ or Saved/.
Never commit passwords, API tokens, signing keys or private assets.
