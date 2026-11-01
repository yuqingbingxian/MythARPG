# Remaining non-content foundation backlog

Status labels: `source` = implemented source but uncompiled, `contract` = API only,
`pending` = not coded yet. No system is considered UE-integrated until verified.

| System | v0.3 status | Remaining work for shipping standard |
|---|---|---|
| Combat input/combo/stamina/poise | source v0.2 | Montage/warp/defense/finisher validation |
| Boss data and attack director | source v0.3 | Phase transaction, arena reset, ability pools |
| AI | prototype + director | Perception/StateTree/EQS/path edge cases |
| Save/campaign | source v0.3 | Backup/atomic replace/checksum, migration fixtures |
| Chapter registry | source v0.3 | Async load/travel state machine and failure recovery |
| World persistent actors | source v0.3 | Editor GUID validation, per-chapter state partitions |
| Inventory/equip/currency | source v0.3 | Equipment attribute effects, item use, UI, validation |
| Crafting/shop | source v0.3 | Selling, restock, pricing modifiers, rollback tests |
| Progression/skill DAG | source v0.3 | Skill grant-to-GAS and refund rules |
| Quests and narrative | source v0.3 | Branching quest graph, typed event broker, reward command |
| Interact and checkpoint | source v0.3 | Multi-option targeting, exact spawn registry |
| Audio/gameflow | source v0.3 | Mix states, cinematic input/Sequencer integration |
| UI/HUD/settings | contract | UI routing, HUD ViewModels, persistent settings |
| Editor tooling | pending | Data validators, graph inspectors, persistent ID lint |
| Automated tests | source (unit subset) | PIE and functional/load/save tests in engine |
| Release | pending | Packaging, crash capture, profiling, localization |

Do not add Chapter01 content until generic runtime contracts are stable enough
for a vertical slice. Do not present source-only subsystems as production complete.
