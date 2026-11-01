#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MythSettingsSaveGame.generated.h"

/** Local user preferences are separate from campaign save slots. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythSettingsSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite) int32 SchemaVersion=1;
    UPROPERTY(BlueprintReadWrite) bool bSubtitlesEnabled=true;
    UPROPERTY(BlueprintReadWrite) bool bCameraInvertY=false;
    UPROPERTY(BlueprintReadWrite) bool bReducedCameraShake=false;
    UPROPERTY(BlueprintReadWrite) float CameraSensitivity=1.f;
    UPROPERTY(BlueprintReadWrite) float MasterVolume=1.f;
    UPROPERTY(BlueprintReadWrite) float MusicVolume=.8f;
    UPROPERTY(BlueprintReadWrite) float SfxVolume=1.f;
    UPROPERTY(BlueprintReadWrite) float DialogueVolume=1.f;
};
