#include "MythEncounterDirector.h"
#include "MythEncounterDefinition.h"
#include "MythEnemyCharacter.h"
#include "MythCombatCharacter.h"
#include "MythSaveSubsystem.h"
#include "MythRewardSubsystem.h"
#include "MythRewardDefinition.h"
#include "MythEventSubsystem.h"
#include "Engine/TargetPoint.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/AssetManager.h"
AMythEncounterDirector::AMythEncounterDirector(){PrimaryActorTick.bCanEverTick=false;}
FName AMythEncounterDirector::GetClearFlag()const
{return Definition?FName(*(FString(TEXT("Encounter.Cleared."))+Definition->ChapterId.ToString()+TEXT(".")+Definition->EncounterId.ToString())):NAME_None;}
void AMythEncounterDirector::ChangeState(EMythEncounterState NewState)
{State=NewState;OnEncounterStateChanged.Broadcast(State,WaveIndex);}
void AMythEncounterDirector::BeginPlay()
{
    Super::BeginPlay();
    if(!Definition || !Definition->bPermanentClear || !GetGameInstance())return;
    if(UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>())
        if(Save->GetSnapshot().WorldValues.FindRef(GetClearFlag())!=0)
            ChangeState(EMythEncounterState::Completed);
}
void AMythEncounterDirector::EndPlay(const EEndPlayReason::Type Reason)
{
    if(PlayerRef.IsValid())PlayerRef->OnCharacterDeath.RemoveDynamic(this,&AMythEncounterDirector::OnPlayerDeath);
    AbortEncounter();Super::EndPlay(Reason);
}
bool AMythEncounterDirector::BeginEncounter(AMythCombatCharacter* Player)
{
    FString Error;
    if(State!=EMythEncounterState::Idle || !Player || Player->IsDead() || !Definition ||
       !Definition->Validate(Error) || !GetWorld())return false;
    TArray<FSoftObjectPath> Paths;
    for(const FMythEncounterWave& Wave:Definition->Waves)
        for(const FMythEncounterSpawn& Spawn:Wave.Spawns)
            Paths.AddUnique(Spawn.EnemyClass.ToSoftObjectPath());
    if(Paths.IsEmpty())return false;
    PlayerRef=Player;
    Player->OnCharacterDeath.AddUniqueDynamic(this,&AMythEncounterDirector::OnPlayerDeath);
    ChangeState(EMythEncounterState::Preparing);
    // Preload all wave classes before spawning. Keep handle alive across wave transitions.
    AssetPreload=UAssetManager::GetStreamableManager().RequestAsyncLoad(
        Paths,FStreamableDelegate::CreateUObject(this,&AMythEncounterDirector::OnEncounterClassesReady));
    if(!AssetPreload)
    {
        AbortEncounter();ChangeState(EMythEncounterState::Failed);
        return false;
    }
    // Cached classes can complete synchronously; guard against a missed delegate.
    if(AssetPreload->HasLoadCompleted())OnEncounterClassesReady();
    return true;
}
void AMythEncounterDirector::OnEncounterClassesReady()
{
    if(State!=EMythEncounterState::Preparing)return; // Cancelled/restarted async request.
    if(!PlayerRef.IsValid() || PlayerRef->IsDead() || !AssetPreload || !AssetPreload->HasLoadCompleted())
    {
        AbortEncounter();ChangeState(EMythEncounterState::Failed);
        return;
    }
    // TSoftClassPtr::Get never performs blocking disk I/O.
    for(const FMythEncounterWave& Wave:Definition->Waves)
        for(const FMythEncounterSpawn& Spawn:Wave.Spawns)
            if(!Spawn.EnemyClass.Get())
            {
                AbortEncounter();ChangeState(EMythEncounterState::Failed);
                return;
            }
    ChangeState(EMythEncounterState::Running);
    if(State!=EMythEncounterState::Running)return; // Blueprint observer may have cancelled the encounter.
    if(!StartWave(0))
    {
        AbortEncounter();ChangeState(EMythEncounterState::Failed);
    }
}
void AMythEncounterDirector::AbortEncounter()
{
    if(PlayerRef.IsValid())PlayerRef->OnCharacterDeath.RemoveDynamic(this,&AMythEncounterDirector::OnPlayerDeath);
    PlayerRef.Reset();
    ClearSpawnedActors();
    if(AssetPreload)
    {
        AssetPreload->CancelHandle();
        AssetPreload.Reset();
    }
    WaveIndex=INDEX_NONE;
}
void AMythEncounterDirector::ClearSpawnedActors()
{
    TArray<TObjectPtr<AMythEnemyCharacter>> ToRemove=MoveTemp(ActiveEnemies);
    ActiveEnemies.Empty();
    for(AMythEnemyCharacter* Enemy:ToRemove)
    {
        if(!IsValid(Enemy))continue;
        Enemy->OnCharacterDeath.RemoveDynamic(this,&AMythEncounterDirector::OnEnemyDeath);
        Enemy->Destroy();
    }
}
bool AMythEncounterDirector::ResetEncounter()
{
    if(State==EMythEncounterState::Completed && Definition && Definition->bPermanentClear)return false;
    AbortEncounter();
    ChangeState(EMythEncounterState::Idle);
    return true;
}
bool AMythEncounterDirector::StartWave(int32 Index)
{
    if(!Definition || !Definition->Waves.IsValidIndex(Index) || !GetWorld())return false;
    WaveIndex=Index;
    const FMythEncounterWave& Wave=Definition->Waves[Index];
    TArray<TSubclassOf<AMythEnemyCharacter>> LoadedClasses;
    for(const FMythEncounterSpawn& Entry:Wave.Spawns)
    {
        UClass* Class=Entry.EnemyClass.Get(); // Preloaded before Running; never sync-load on the combat path.
        if(!Class || !Class->IsChildOf(AMythEnemyCharacter::StaticClass()))return false;
        for(int32 I=0;I<Entry.Count;++I)LoadedClasses.Add(Class);
    }
    if(LoadedClasses.IsEmpty())return false;
    // All class loads happen before spawning: never spawn half a wave due to asset failure.
    for(int32 I=0;I<LoadedClasses.Num();++I)
    {
        const ATargetPoint* Point=SpawnPoints.IsEmpty()?nullptr:SpawnPoints[I%SpawnPoints.Num()];
        const FTransform Spawn=IsValid(Point)?Point->GetActorTransform():GetActorTransform();
        AMythEnemyCharacter* Enemy=GetWorld()->SpawnActor<AMythEnemyCharacter>(LoadedClasses[I],Spawn);
        if(!Enemy){ClearSpawnedActors();return false;}
        ActiveEnemies.Add(Enemy);
        Enemy->OnCharacterDeath.AddUniqueDynamic(this,&AMythEncounterDirector::OnEnemyDeath);
    }
    OnEncounterStateChanged.Broadcast(State,WaveIndex);
    return true;
}
void AMythEncounterDirector::OnEnemyDeath(AMythCombatCharacter* DeadActor)
{
    if(State!=EMythEncounterState::Running || !DeadActor)return;
    AMythEnemyCharacter* Enemy=Cast<AMythEnemyCharacter>(DeadActor);
    if(!Enemy || !ActiveEnemies.Contains(Enemy))return;
    Enemy->OnCharacterDeath.RemoveDynamic(this,&AMythEncounterDirector::OnEnemyDeath);
    Enemy->SetLifeSpan(5.f); // Give death animation time; no live encounter reference remains.
    ActiveEnemies.RemoveSingleSwap(Enemy);
    if(!ActiveEnemies.IsEmpty())return;
    if(Definition && Definition->Waves.IsValidIndex(WaveIndex+1))
    {
        if(!StartWave(WaveIndex+1))
        {AbortEncounter();ChangeState(EMythEncounterState::Failed);}
    }
    else if(!FinishEncounter())
    {AbortEncounter();ChangeState(EMythEncounterState::Failed);}
}
void AMythEncounterDirector::OnPlayerDeath(AMythCombatCharacter* DeadActor)
{
    if(DeadActor==PlayerRef.Get() &&
       (State==EMythEncounterState::Running || State==EMythEncounterState::Preparing))ResetEncounter();
}
bool AMythEncounterDirector::FinishEncounter()
{
    if(State!=EMythEncounterState::Running || !Definition || !GetGameInstance())return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(Definition->bPermanentClear)S.WorldValues.Add(GetClearFlag(),1);
    if(!Definition->ClearReward.IsNull())
    {
        UMythRewardSubsystem* Rewards=GetGameInstance()->GetSubsystem<UMythRewardSubsystem>();
        const UMythRewardDefinition* Reward=Definition->ClearReward.LoadSynchronous();
        if(!Rewards || !Reward || !Rewards->TryGrantToSnapshot(S,Reward))return false;
    }
    if(!Save->SetSnapshot(S))return false;
    if(PlayerRef.IsValid())PlayerRef->OnCharacterDeath.RemoveDynamic(this,&AMythEncounterDirector::OnPlayerDeath);
    PlayerRef.Reset();
    if(AssetPreload){AssetPreload->ReleaseHandle();AssetPreload.Reset();}
    ChangeState(EMythEncounterState::Completed);
    if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
    {
        FMythGameplaySignal Signal;Signal.EventName=TEXT("EncounterCleared");
        Signal.TargetId=Definition->EncounterId;Signal.ChapterId=Definition->ChapterId;
        Events->PublishSignal(Signal);
    }
    return true;
}
