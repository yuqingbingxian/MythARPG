#include "MythAnimNotifyState_DodgeCancelWindow.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
#include "Components/SkeletalMeshComponent.h"
void UMythAnimNotifyState_DodgeCancelWindow::NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Ref)
{
    Super::NotifyBegin(MeshComp,Animation,Duration,Ref);
    if(AMythCombatCharacter* Owner=MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()):nullptr)
        if(UMythActionComponent* Action=Owner->GetActionComponent())Action->OpenDodgeCancelWindow();
}
void UMythAnimNotifyState_DodgeCancelWindow::NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Ref)
{
    if(AMythCombatCharacter* Owner=MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()):nullptr)
        if(UMythActionComponent* Action=Owner->GetActionComponent())Action->CloseDodgeCancelWindow();
    Super::NotifyEnd(MeshComp,Animation,Ref);
}
