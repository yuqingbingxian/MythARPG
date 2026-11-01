#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythEventSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMythGameplaySignal
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName EventName=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName TargetId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName ChapterId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Count=1;
    /** Optional persistent one-time receipt. Blank allows repeatable events. */
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName StableReceiptId=NAME_None;
};
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMythBossDefeated,FName /*BossId*/,FName /*ChapterId*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMythGameplaySignal,const FMythGameplaySignal&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMythGameplaySignalBP,FMythGameplaySignal,Signal);

/** Typed fact broker: no direct gameplay implementation or cross-module dependency. */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythEventSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    FOnMythBossDefeated OnBossDefeated;
    FOnMythGameplaySignal OnSignal;
    UPROPERTY(BlueprintAssignable) FMythGameplaySignalBP OnSignalBP;
    UFUNCTION(BlueprintCallable,Category="Events") bool PublishSignal(const FMythGameplaySignal& Signal);
    void PublishBossDefeated(FName BossId,FName ChapterId);
    UFUNCTION(BlueprintCallable,Category="Events") void DeliverPending();
private:
    bool bDispatching=false;
};
