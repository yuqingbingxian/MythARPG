# v0.9 — source quality fixes and automated duplicate scan

## Fixes

- Remove duplicate `GuardAction` and `ParryAction` property declarations in `AMythPlayerCharacter`.
- Remove duplicate guard/parry handler method declarations and definitions.
- Remove duplicate `EnhancedInputComponent::BindAction` calls for guard/parry actions.
- Remove duplicate include of `MythDefenseComponent.h`.
- Remove duplicate `DefenseComponent` member, getter and `CreateDefaultSubobject` call in `AMythCombatCharacter`.
- Refine static duplicate detection to respect distinct `USTRUCT` and `UCLASS` declarations.
- Add `Scripts/verify_cpp_duplicates_v09.py` into GitHub Actions for a conservative duplicate-method/property scan.

## Important

These duplicate entries were a real potential C++ compilation blocker missed by previous token-only static checks. The new scan is **not** a C++ parser or UnrealBuildTool/UHT replacement. Unreal engine compile is still required to evaluate all plugins and generated headers.
