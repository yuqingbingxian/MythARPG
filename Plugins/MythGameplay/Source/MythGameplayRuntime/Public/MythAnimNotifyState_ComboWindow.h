#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MythAnimNotifyState_ComboWindow.generated.h"
UCLASS(meta=(DisplayName="Myth Combo Window"))
class MYTHGAMEPLAYRUNTIME_API UMythAnimNotifyState_ComboWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,float TotalDuration,const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;
};
