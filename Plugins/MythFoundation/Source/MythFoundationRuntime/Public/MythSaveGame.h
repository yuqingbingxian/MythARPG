#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MythSaveGame.generated.h"

/** V1 format. Stable names are content contracts; never rename IDs casually. */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly)
    int32 SchemaVersion = 1;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly)
    FName ChapterId = TEXT("CH01");

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly)
    bool bHasPlayerState = false;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly)
    FTransform PlayerTransform = FTransform::Identity;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly)
    float PlayerHealth = 100.f;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly)
    TSet<FName> DefeatedBossIds;
};
