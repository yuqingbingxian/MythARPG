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
    ClearSpawnedActors();Super::EndPlay(Reason);
}
bool AMythEncounterDirector::BeginEncounter(AMythCombatCharacter* Player)
{
    FString Error;
    if(State!=EMythEncounterState::Idle || !Player || Player->IsDead() || !Definition ||
       !Definition->Validate(Error) || !GetWorld())return false;
    PlayerRef=Player;Player->OnCharacterDeath.AddUniqueDynamic(this,&AMythEncounterDirector::OnPlayerDeath);
    ChangeState(EMythEncounterState::Running);
    if(!StartWave(0))
    {ResetEncounter();ChangeState(EMythEncounterState::Failed);return false;}
    return true;
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
    if(PlayerRef.IsValid())PlayerRef->OnCharacterDeath.RemoveDynamic(this,&AMythEncounterDirector::OnPlayerDeath);
    PlayerRef.Reset();ClearSpawnedActors();WaveIndex=INDEX_NONE;
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
        UClass* Class=Entry.EnemyClass.LoadSynchronous();
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
        if(!StartWave(WaveIndex+1))ChangeState(EMythEncounterState::Failed);
    }
    else if(!FinishEncounter())ChangeState(EMythEncounterState::Failed);
}
void AMythEncounterDirector::OnPlayerDeath(AMythCombatCharacter* DeadActor)
{
    if(DeadActor==PlayerRef.Get() && State==EMythEncounterState::Running)ResetEncounter();
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
    PlayerRef.Reset();ChangeState(EMythEncounterState::Completed);
    if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
    {
        FMythGameplaySignal Signal;Signal.EventName=TEXT("EncounterCleared");
        Signal.TargetId=Definition->EncounterId;Signal.ChapterId=Definition->ChapterId;
        Events->PublishSignal(Signal);
    }
    return true;
}
