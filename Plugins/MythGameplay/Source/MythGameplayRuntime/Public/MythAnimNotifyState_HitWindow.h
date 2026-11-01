#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MythAnimNotifyState_HitWindow.generated.h"

/** Place on the attack Montage. Gameplay is executed by character's action component. */
UCLASS(meta=(DisplayName="Myth Hit Window"))
class MYTHGAMEPLAYRUNTIME_API UMythAnimNotifyState_HitWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float TotalDuration,const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;
};
