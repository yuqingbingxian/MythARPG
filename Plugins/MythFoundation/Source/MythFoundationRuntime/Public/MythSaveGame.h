#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MythCampaignTypes.h"
#include "MythSaveGame.generated.h"

UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly) int32 SchemaVersion = 3;
    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly) FMythCampaignSnapshot Campaign;

    // V1 compatibility fields: do not remove until migration tests cover production saves.
    UPROPERTY(SaveGame) FName ChapterId = TEXT("CH01");
    UPROPERTY(SaveGame) bool bHasPlayerState = false;
    UPROPERTY(SaveGame) FTransform PlayerTransform = FTransform::Identity;
    UPROPERTY(SaveGame) float PlayerHealth = 100.f;
    UPROPERTY(SaveGame) TSet<FName> DefeatedBossIds;
};
