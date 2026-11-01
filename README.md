# MythARPG — Unreal Engine 5 shared gameplay framework (v0.3)

**Source prototype, NOT a compiled / tested commercial game.**

The repository implements reusable, non-chapter runtime foundation for a
single-player third-person action RPG in C++ with Blueprint/Data Asset extension
points. v0.3 adds save snapshots, chapter registry, inventory, equipment effects,
currency, shop, crafting, skill progression, quests, dialogue, interaction,
checkpoints, world persistence, audio and screen-flow services. v0.2 combat
movement, lock-on, GAS attacks, stamina, poise and the enemy/Boss prototype remain.

- [v0.3 system architecture](Docs/09_FOUNDATION_V03.md)
- [Non-content framework backlog](Docs/10_SYSTEM_BACKLOG.md)
- [Unreal editor setup for the combat prototype](Docs/07_EDITOR_SETUP_V02.md)
- [Integration QA](Docs/08_INTEGRATION_QA.md)

## Important

There are no chapter-specific maps, story assets, NPC setups, boss art, or
playable first-chapter content in this iteration. All C++ changes require
UnrealBuildTool/UHT compilation and engine integration tests before any claim of
runtime correctness. Python structure checks are not substitutes for those.

```bash
python Scripts/verify_structure.py
python Scripts/verify_systems.py
```

Do not commit `Binaries/`, `Intermediate/`, `Saved/` or `DerivedDataCache/`.
Add future `.uasset`/`.umap` files via Git LFS per `.gitattributes`.

### v0.4 foundation (source-only)

See [`Docs/11_FOUNDATION_V04.md`](Docs/11_FOUNDATION_V04.md) for crash-tolerant PC saves,
chapter travel and editor content audits. Requires an actual UE 5.8 build and runtime tests.
