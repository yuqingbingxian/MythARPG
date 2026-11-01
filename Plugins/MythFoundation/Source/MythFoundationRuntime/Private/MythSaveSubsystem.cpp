#include "MythSaveSubsystem.h"
#include "MythSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UMythSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ReloadFromDisk();
}

bool UMythSaveSubsystem::ReloadFromDisk()
{
    CurrentSave = Cast<UMythSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!CurrentSave)
    {
        CurrentSave = Cast<UMythSaveGame>(UGameplayStatics::CreateSaveGameObject(UMythSaveGame::StaticClass()));
    }
    return IsValid(CurrentSave);
}

bool UMythSaveSubsystem::CommitToDisk()
{
    return CurrentSave && UGameplayStatics::SaveGameToSlot(CurrentSave, SlotName, 0);
}

bool UMythSaveSubsystem::SavePlayerState(const FTransform& Transform, float Health)
{
    if (!CurrentSave) return false;
    CurrentSave->PlayerTransform = Transform;
    CurrentSave->PlayerHealth = FMath::Max(0.f, Health);
    CurrentSave->bHasPlayerState = true;
    return CommitToDisk();
}

bool UMythSaveSubsystem::RecordBossDefeat(FName BossId)
{
    if (!CurrentSave || BossId.IsNone()) return false;
    if (CurrentSave->DefeatedBossIds.Contains(BossId)) return true;
    CurrentSave->DefeatedBossIds.Add(BossId);
    return CommitToDisk();
}

bool UMythSaveSubsystem::IsBossDefeated(FName BossId) const
{
    return CurrentSave && CurrentSave->DefeatedBossIds.Contains(BossId);
}

bool UMythSaveSubsystem::GetSavedPlayerState(FTransform& OutTransform, float& OutHealth) const
{
    if (!CurrentSave || !CurrentSave->bHasPlayerState) return false;
    OutTransform = CurrentSave->PlayerTransform;
    OutHealth = CurrentSave->PlayerHealth;
    return true;
}
