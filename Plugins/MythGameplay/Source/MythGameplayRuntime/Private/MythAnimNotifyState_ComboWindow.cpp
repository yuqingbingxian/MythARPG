#include "MythAnimNotifyState_ComboWindow.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
#include "Components/SkeletalMeshComponent.h"
void UMythAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Ref)
{
    Super::NotifyBegin(MeshComp,Animation,Duration,Ref);
    if (AMythCombatCharacter* Char = MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()) : nullptr)
        if (UMythActionComponent* Action=Char->GetActionComponent()) Action->OpenComboWindow();
}
void UMythAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Ref)
{
    if (AMythCombatCharacter* Char = MeshComp ? Cast<AMythCombatCharacter>(MeshComp->GetOwner()) : nullptr)
        if (UMythActionComponent* Action=Char->GetActionComponent()) Action->CloseComboWindow();
    Super::NotifyEnd(MeshComp,Animation,Ref);
}
