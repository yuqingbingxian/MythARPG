#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCheckpointSubsystem.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythCheckpointChanged,FName,ChapterId,FName,CheckpointId);
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythCheckpointSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable) FMythCheckpointChanged OnCheckpointActivated;
    UFUNCTION(BlueprintCallable) bool ActivateCheckpoint(FName ChapterId,FName CheckpointId,AActor* PlayerActor,float PlayerHealth);
    UFUNCTION(BlueprintPure) FName GetActiveCheckpoint()const;
};
