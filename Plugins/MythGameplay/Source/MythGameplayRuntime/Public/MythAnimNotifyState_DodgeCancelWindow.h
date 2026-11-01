#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MythAnimNotifyState_DodgeCancelWindow.generated.h"
/** Optional authored cancel window for attack montages. */
UCLASS(meta=(DisplayName="Myth Dodge Cancel Window"))
class MYTHGAMEPLAYRUNTIME_API UMythAnimNotifyState_DodgeCancelWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float Duration,const FAnimNotifyEventReference& Ref)override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& Ref)override;
};
