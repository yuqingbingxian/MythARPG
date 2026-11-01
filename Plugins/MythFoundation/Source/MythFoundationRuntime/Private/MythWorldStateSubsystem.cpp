#include "MythWorldStateSubsystem.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"

int32 UMythWorldStateSubsystem::ReadValue(FName Key) const
{
    const UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if (!Save || Key.IsNone()) return 0;
    return Save->GetSnapshot().WorldValues.FindRef(Key);
}

bool UMythWorldStateSubsystem::WriteValue(FName Key,int32 Value,bool bSaveNow)
{
    if (Key.IsNone()) return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if (!Save) return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if (S.WorldValues.FindRef(Key)==Value) return true;
    S.WorldValues.Add(Key,Value);
    if (!Save->SetSnapshot(S,bSaveNow)) return false;
    OnWorldValueChanged.Broadcast(Key,Value);
    return true;
}

bool UMythWorldStateSubsystem::IncrementValue(FName Key,int32 Delta)
{
    const int64 Next=static_cast<int64>(ReadValue(Key))+Delta;
    if (Next<MIN_int32 || Next>MAX_int32) return false;
    return WriteValue(Key,static_cast<int32>(Next));
}
