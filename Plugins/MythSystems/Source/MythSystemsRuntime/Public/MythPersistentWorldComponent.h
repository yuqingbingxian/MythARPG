#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythPersistentWorldComponent.generated.h"
/** Assign unique authored ID per persistent placed actor. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHSYSTEMSRUNTIME_API UMythPersistentWorldComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythPersistentWorldComponent();
    UPROPERTY(EditInstanceOnly,BlueprintReadOnly,Category="Persistence") FName PersistentId=NAME_None;
    UFUNCTION(BlueprintPure) bool IsActivated()const;
    UFUNCTION(BlueprintCallable) bool SetActivated(bool bActivated=true);
    UFUNCTION(BlueprintImplementableEvent) void OnRestored(bool bActivated);
protected:
    virtual void BeginPlay() override;
};
