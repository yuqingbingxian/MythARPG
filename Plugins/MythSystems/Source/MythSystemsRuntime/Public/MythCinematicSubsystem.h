#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCinematicSubsystem.generated.h"
class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythCinematicFinished,FName,CinematicId,bool,bSkipped);

/** Controls Sequencer lifecycle; story state is committed by Narrative, not by Sequence tracks. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythCinematicSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UPROPERTY(BlueprintAssignable,Category="Myth|Cinematic") FMythCinematicFinished OnCinematicFinished;
    UFUNCTION(BlueprintCallable,Category="Myth|Cinematic") bool PlayCinematic(FName CinematicId,ULevelSequence* Sequence);
    UFUNCTION(BlueprintCallable,Category="Myth|Cinematic") bool SkipCinematic();
    UFUNCTION(BlueprintPure,Category="Myth|Cinematic") bool IsCinematicPlaying()const{return ActivePlayer!=nullptr;}
private:
    UPROPERTY(Transient) TObjectPtr<ULevelSequencePlayer> ActivePlayer;
    UPROPERTY(Transient) TObjectPtr<ALevelSequenceActor> ActiveActor;
    FName ActiveId=NAME_None;
    FDelegateHandle PreMapHandle;
    bool bSkipping=false;
    UFUNCTION()void OnSequenceFinished();
    void Finish(bool bSkipped);
    void BeforeMapLoad(const FString& MapName);
};
