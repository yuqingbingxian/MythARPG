#include "MythNarrativeCommandDefinition.h"
bool UMythNarrativeCommandDefinition::Validate(FString& Error)const
{
    if(ReceiptId.IsNone()){Error=TEXT("Missing stable ReceiptId");return false;}
    if(Commands.IsEmpty()){Error=TEXT("Command sequence is empty");return false;}
    for(const FMythNarrativeCommand& Command:Commands)
    {
        if(Command.Opcode==EMythNarrativeOpcode::GrantReward)
        {if(Command.Reward.IsNull()){Error=TEXT("GrantReward has no reward asset");return false;}}
        else if(Command.TargetId.IsNone())
        {Error=TEXT("A command is missing TargetId");return false;}
    }
    Error.Reset();return true;
}
