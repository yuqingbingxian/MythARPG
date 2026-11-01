#include "MythEnemyAIController.h"
#include "MythAICombatProfile.h"
#include "MythEnemyCharacter.h"
#include "MythPlayerCharacter.h"
#include "MythCombatDirectorSubsystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/StateTreeAIComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

AMythEnemyAIController::AMythEnemyAIController()
{
    PrimaryActorTick.bCanEverTick=false;
    StateTreeBrain=CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeBrain"));
    StateTreeBrain->SetStartLogicAutomatically(false);
    Perception=CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("SightPerception"));
    SightConfig=CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius=1400.f;
    SightConfig->LoseSightRadius=1700.f;
    SightConfig->PeripheralVisionAngleDegrees=95.f;
    SightConfig->SetMaxAge(3.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies=true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals=true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies=true;
    Perception->ConfigureSense(*SightConfig);
    Perception->SetDominantSense(UAISense_Sight::StaticClass());
    SetPerceptionComponent(*Perception);
    Perception->OnTargetPerceptionUpdated.AddDynamic(this,&AMythEnemyAIController::OnTargetStimulus);
}
void AMythEnemyAIController::OnPossess(APawn* Pawn)
{
    Super::OnPossess(Pawn);
    if(Pawn){HomeLocation=Pawn->GetActorLocation();DecisionRng.Initialize(GetTypeHash(Pawn->GetFName()));}
    PendingQueryId=INDEX_NONE;
    bHasTacticalPosition=false;
    if(bUseStateTree)
        StateTreeBrain->StartLogic();
    else
        GetWorldTimerManager().SetTimer(DecisionTimer,this,&AMythEnemyAIController::EvaluateCombat,.25f,true);
}
void AMythEnemyAIController::OnUnPossess()
{
    GetWorldTimerManager().ClearTimer(DecisionTimer);
    if(StateTreeBrain && StateTreeBrain->IsRunning())StateTreeBrain->StopLogic(TEXT("Unpossess"));
    if(GetWorld())
        if(UEnvQueryManager* Manager=UEnvQueryManager::GetCurrent(GetWorld()))
            Manager->SilentlyRemoveAllQueriesByQuerier(*this);
    PendingQueryId=INDEX_NONE;
    bHasTacticalPosition=false;
    ForgetCombatTarget();
    Super::OnUnPossess();
}
void AMythEnemyAIController::OnTargetStimulus(AActor* Actor,FAIStimulus Stimulus)
{
    AMythCombatCharacter* Seen=Cast<AMythCombatCharacter>(Actor);
    if(!Seen || Seen->IsDead() || Seen->GetFaction()!=EMythFaction::Player || !GetWorld())return;
    if(Stimulus.WasSuccessfullySensed())
    {
        Target=Seen;bTargetVisible=true;
        LastKnownLocation=Seen->GetActorLocation();
        LastSeenAt=GetWorld()->GetTimeSeconds();
    }
    else if(Target.Get()==Seen)
    {bTargetVisible=false;LastKnownLocation=Stimulus.StimulusLocation;}
}
void AMythEnemyAIController::ForgetCombatTarget()
{
    if(UWorld* World=GetWorld())
        if(UMythCombatDirectorSubsystem* Director=World->GetSubsystem<UMythCombatDirectorSubsystem>())
            Director->ReleaseAttack(GetPawn());
    Target.Reset();bTargetVisible=false;StopMovement();
}
void AMythEnemyAIController::EvaluateCombat()
{
    AMythEnemyCharacter* Enemy=Cast<AMythEnemyCharacter>(GetPawn());
    if(!Enemy || !GetWorld() || Enemy->IsDead()){ForgetCombatTarget();return;}
    const double Now=GetWorld()->GetTimeSeconds();
    if(!Target.IsValid() && bDebugAcquirePlayerWithoutSight)
        if(AMythPlayerCharacter* Player=Cast<AMythPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0)))
            if(!Player->IsDead() && FVector::Dist2D(Enemy->GetActorLocation(),Player->GetActorLocation())<=Enemy->GetAggroRange())
            {Target=Player;bTargetVisible=true;LastSeenAt=Now;LastKnownLocation=Player->GetActorLocation();}
    AMythCombatCharacter* Victim=Target.Get();
    if(!Victim || Victim->IsDead() || FVector::Dist2D(HomeLocation,Enemy->GetActorLocation())>
        (CombatProfile?CombatProfile->HomeLeashDistance:2000.f) ||
        Now-LastSeenAt>(CombatProfile?CombatProfile->ForgetTargetSeconds:4.f))
    {ForgetCombatTarget();MoveToLocation(HomeLocation,65.f);return;}
    if(bTargetVisible)
    {LastSeenAt=Now;LastKnownLocation=Victim->GetActorLocation();}
    else
    {MoveToLocation(LastKnownLocation,65.f);return;}
    const float Distance=FVector::Dist2D(Enemy->GetActorLocation(),Victim->GetActorLocation());
    if(Distance>Enemy->GetAttackRange())
    {MoveToActor(Victim,FMath::Max(75.f,CombatProfile?CombatProfile->PreferredFightDistance:Enemy->GetAttackRange()*.8f));return;}
    StopMovement();
    FVector Facing=Victim->GetActorLocation()-Enemy->GetActorLocation();
    if(!Facing.IsNearlyZero())Enemy->SetActorRotation(Facing.Rotation());
    const FMythAIAttackOption* Choice=CombatProfile?CombatProfile->ChooseAttack(Distance,AttackCooldowns,Now,DecisionRng):nullptr;
    if(!Choice && CombatProfile)return;
    if(!Choice && Now-LastAttackAt<1.25)return;
    UMythCombatDirectorSubsystem* Director=GetWorld()->GetSubsystem<UMythCombatDirectorSubsystem>();
    const float Reservation=Choice?FMath::Max(0.5f,Choice->Cooldown):1.5f;
    if(Director && !Director->TryReserveAttack(Enemy,Victim,Reservation))return;
    const bool bStarted=Choice&&Choice->bHeavyAttack?Enemy->RequestHeavyAbility():Enemy->RequestMeleeAbility();
    if(bStarted)
    {LastAttackAt=Now;if(Choice)AttackCooldowns.Add(Choice->AttackId,Now);}
    else if(Director)Director->ReleaseAttack(Enemy);
}

void AMythEnemyAIController::EvaluateCombatStep()
{
    // One invocation per StateTree tick/task event. The legacy mode calls the same method via timer.
    EvaluateCombat();
}

bool AMythEnemyAIController::RequestTacticalPosition()
{
    if(!TacticalPositionQuery || !GetPawn() || !GetWorld() || PendingQueryId!=INDEX_NONE)
        return false;
    bHasTacticalPosition=false;
    FEnvQueryRequest Request(TacticalPositionQuery,this);
    PendingQueryId=Request.Execute(EEnvQueryRunMode::SingleResult,this,
        &AMythEnemyAIController::OnTacticalQueryFinished);
    return PendingQueryId!=INDEX_NONE;
}

void AMythEnemyAIController::OnTacticalQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if(!Result.IsValid() || Result->QueryID!=PendingQueryId)return; // Stale completion after reset.
    PendingQueryId=INDEX_NONE;
    bHasTacticalPosition=false;
    if(!GetPawn() || Result->IsAborted() || !Result->IsSuccessful() || Result->Items.IsEmpty())return;
    const FVector NewPosition=Result->GetItemAsLocation(0);
    if(NewPosition.ContainsNaN())return;
    TacticalPosition=NewPosition;
    bHasTacticalPosition=true;
}

bool AMythEnemyAIController::MoveToTacticalPosition(float AcceptanceRadius)
{
    if(!GetPawn() || !bHasTacticalPosition)return false;
    return MoveToLocation(TacticalPosition,FMath::Max(10.f,AcceptanceRadius))!=
        EPathFollowingRequestResult::Failed;
}

void AMythEnemyAIController::CancelTacticalQuery()
{
    if(GetWorld())
        if(UEnvQueryManager* Manager=UEnvQueryManager::GetCurrent(GetWorld()))
            Manager->SilentlyRemoveAllQueriesByQuerier(*this);
    PendingQueryId=INDEX_NONE;bHasTacticalPosition=false;
}
