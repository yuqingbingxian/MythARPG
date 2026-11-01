#include "MythPersistentWorldComponent.h"
#include "MythWorldStateSubsystem.h"
#include "Engine/GameInstance.h"
UMythPersistentWorldComponent::UMythPersistentWorldComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UMythPersistentWorldComponent::BeginPlay()
{
    Super::BeginPlay();
    if(PersistentId.IsNone())
    {UE_LOG(LogTemp,Warning,TEXT("Persistent object %s is missing its stable PersistentId"),*GetOwner()->GetPathName());return;}
    OnRestored(IsActivated());
}
bool UMythPersistentWorldComponent::IsActivated()const
{
    if(!GetWorld() || !GetWorld()->GetGameInstance() || PersistentId.IsNone())return false;
    UMythWorldStateSubsystem* State=GetWorld()->GetGameInstance()->GetSubsystem<UMythWorldStateSubsystem>();
    return State && State->HasFlag(PersistentId);
}
bool UMythPersistentWorldComponent::SetActivated(bool bActivated)
{
    if(!GetWorld() || !GetWorld()->GetGameInstance() || PersistentId.IsNone())return false;
    UMythWorldStateSubsystem* State=GetWorld()->GetGameInstance()->GetSubsystem<UMythWorldStateSubsystem>();
    if(!State || !State->WriteValue(PersistentId,bActivated?1:0))return false;
    OnRestored(bActivated);
    return true;
}
