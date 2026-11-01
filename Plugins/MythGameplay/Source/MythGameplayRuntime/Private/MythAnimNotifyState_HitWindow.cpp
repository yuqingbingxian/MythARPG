#include "MythAnimNotifyState_HitWindow.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
#include "Components/SkeletalMeshComponent.h"
void UMythAnimNotifyState_HitWindow::NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float TotalDuration,const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp,Animation,TotalDuration,EventReference);
    if (AMythCombatCharacter* Char = MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()) : nullptr)
        if (UMythActionComponent* Action = Char->GetActionComponent()) Action->OpenHitWindow();
}
void UMythAnimNotifyState_HitWindow::NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
    if (AMythCombatCharacter* Char = MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()) : nullptr)
        if (UMythActionComponent* Action = Char->GetActionComponent()) Action->CloseHitWindow();
    Super::NotifyEnd(MeshComp,Animation,EventReference);
}
