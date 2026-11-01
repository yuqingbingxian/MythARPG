#include "MythDialogueDefinition.h"
const FMythDialogueNode* UMythDialogueDefinition::FindNode(FName Id)const
{return Nodes.FindByPredicate([Id](const FMythDialogueNode& N){return N.NodeId==Id;});}
bool UMythDialogueDefinition::HasNode(FName Id)const{return FindNode(Id)!=nullptr;}
