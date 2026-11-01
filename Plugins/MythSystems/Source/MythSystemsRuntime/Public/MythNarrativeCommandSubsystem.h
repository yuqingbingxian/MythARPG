#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCampaignTypes.h"
#include "MythNarrativeCommandSubsystem.generated.h"
class UMythNarrativeCommandDefinition;

/** Stages a whole sequence in an isolated snapshot before committing once. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythNarrativeCommandSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable,Category="Myth|Narrative")bool RunSequence(UMythNarrativeCommandDefinition* Definition);
    bool ApplyToSnapshot(const UMythNarrativeCommandDefinition* Definition,FMythCampaignSnapshot& Snapshot)const;
    void PublishEvents(const UMythNarrativeCommandDefinition* Definition)const;
};
