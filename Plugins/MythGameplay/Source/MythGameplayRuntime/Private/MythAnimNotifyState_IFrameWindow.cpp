#include "MythAnimNotifyState_IFrameWindow.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
#include "Components/SkeletalMeshComponent.h"
void UMythAnimNotifyState_IFrameWindow::NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Ref)
{
    Super::NotifyBegin(MeshComp,Animation,Duration,Ref);
    if(AMythCombatCharacter* Owner=MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()):nullptr)
        if(UMythActionComponent* Action=Owner->GetActionComponent())Action->OpenDodgeIFrames();
}
void UMythAnimNotifyState_IFrameWindow::NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Ref)
{
    if(AMythCombatCharacter* Owner=MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()):nullptr)
        if(UMythActionComponent* Action=Owner->GetActionComponent())Action->CloseDodgeIFrames();
    Super::NotifyEnd(MeshComp,Animation,Ref);
}
