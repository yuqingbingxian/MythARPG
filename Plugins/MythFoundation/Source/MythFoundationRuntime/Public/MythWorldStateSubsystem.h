#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythWorldStateSubsystem.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythWorldValueChanged,FName,Key,int32,Value);

/** Durable world-state keys, persisted in the campaign snapshot. Zero means false/unset. */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythWorldStateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable,Category="World") FMythWorldValueChanged OnWorldValueChanged;
    UFUNCTION(BlueprintPure,Category="World") int32 ReadValue(FName Key) const;
    UFUNCTION(BlueprintPure,Category="World") bool HasFlag(FName Key) const { return ReadValue(Key)!=0; }
    UFUNCTION(BlueprintCallable,Category="World") bool WriteValue(FName Key,int32 Value,bool bSaveNow=false);
    UFUNCTION(BlueprintCallable,Category="World") bool IncrementValue(FName Key,int32 Delta=1);
};
