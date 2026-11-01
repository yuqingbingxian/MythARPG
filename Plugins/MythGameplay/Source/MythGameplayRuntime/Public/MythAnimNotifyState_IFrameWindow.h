#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MythAnimNotifyState_IFrameWindow.generated.h"
/** Optional animation-driven dodge i-frame window (enable bDodgeUseAnimationNotifies). */
UCLASS(meta=(DisplayName="Myth Dodge IFrames"))
class MYTHGAMEPLAYRUNTIME_API UMythAnimNotifyState_IFrameWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Ref)override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Ref)override;
};
