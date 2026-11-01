#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythAudioDirectorSubsystem.generated.h"
class UAudioComponent;
class USoundBase;
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythAudioDirectorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) void SetMusic(USoundBase* Music,float Volume=1.f);
    UFUNCTION(BlueprintCallable) void StopMusic(float FadeOutSeconds=.5f);
    UFUNCTION(BlueprintCallable) void PlayUISound(USoundBase* Sound);
private:
    UPROPERTY(Transient) TObjectPtr<UAudioComponent> ActiveMusic;
};
