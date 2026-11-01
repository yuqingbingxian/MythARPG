#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythComboGraph.generated.h"
class UAnimMontage;

UENUM(BlueprintType)
enum class EMythAttackInput : uint8 { Light, Heavy };

/** All gameplay timing is in seconds measured from the start of a node. */
USTRUCT(BlueprintType)
struct FMythComboNode
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") FName NodeId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") EMythAttackInput EntryInput = EMythAttackInput::Light;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") TObjectPtr<UAnimMontage> Montage = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") FName MontageSection = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.1")) float Duration = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float StaminaCost = 12.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float DamageScale = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0")) float PoiseDamage = 20.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") bool bUseAnimationNotifies = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") float HitOpen = .18f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") float HitClose = .38f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") float ComboOpen = .32f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") float ComboClose = .65f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") float DodgeCancelOpen = .42f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") float DodgeCancelClose = .7f;
};

USTRUCT(BlueprintType)
struct FMythComboLink
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") FName From = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") EMythAttackInput Input = EMythAttackInput::Light;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") FName To = NAME_None;
};

UCLASS(BlueprintType)
class MYTHGAMEPLAYRUNTIME_API UMythComboGraph : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") FName EntryLight = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") FName EntryHeavy = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") TArray<FMythComboNode> Nodes;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo") TArray<FMythComboLink> Links;

    const FMythComboNode* FindNode(FName Id) const;
    const FMythComboNode* FindEntry(EMythAttackInput Input) const;
    const FMythComboNode* FindNext(FName Current, EMythAttackInput Input) const;
    bool ValidateGraph(FString& OutError) const;
};
