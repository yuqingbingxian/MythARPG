#include "MythComboGraph.h"

const FMythComboNode* UMythComboGraph::FindNode(FName Id) const
{
    return Nodes.FindByPredicate([Id](const FMythComboNode& N) {return N.NodeId == Id;});
}
const FMythComboNode* UMythComboGraph::FindEntry(EMythAttackInput Input) const
{
    const FName Explicit=Input==EMythAttackInput::Light ? EntryLight : EntryHeavy;
    if (!Explicit.IsNone()) return FindNode(Explicit);
    return Nodes.FindByPredicate([Input](const FMythComboNode& N) {return N.EntryInput == Input;});
}
const FMythComboNode* UMythComboGraph::FindNext(FName Current, EMythAttackInput Input) const
{
    const FMythComboLink* Link = Links.FindByPredicate([Current,Input](const FMythComboLink& L)
    {return L.From == Current && L.Input == Input;});
    return Link ? FindNode(Link->To) : nullptr;
}
bool UMythComboGraph::ValidateGraph(FString& OutError) const
{
    TSet<FName> Known;
    TSet<FString> Edges;
    if (Nodes.IsEmpty()) {OutError = TEXT("Graph has no nodes"); return false;}
    for (const FMythComboNode& N : Nodes)
    {
        if (N.NodeId.IsNone() || Known.Contains(N.NodeId)) {OutError = TEXT("Empty/duplicate NodeId"); return false;}
        Known.Add(N.NodeId);
        if (N.Duration <= 0.f || N.StaminaCost < 0.f || N.HitOpen < 0.f || N.HitClose > N.Duration ||
            N.HitOpen >= N.HitClose || N.ComboOpen < 0.f || N.ComboClose > N.Duration ||
            N.ComboOpen >= N.ComboClose || N.DodgeCancelOpen < 0.f || N.DodgeCancelClose > N.Duration ||
            N.DodgeCancelOpen > N.DodgeCancelClose)
        {OutError = FString::Printf(TEXT("Invalid timing: %s"), *N.NodeId.ToString());return false;}
    }
    if ((!EntryLight.IsNone() && (!Known.Contains(EntryLight) || FindNode(EntryLight)->EntryInput!=EMythAttackInput::Light)) ||
        (!EntryHeavy.IsNone() && (!Known.Contains(EntryHeavy) || FindNode(EntryHeavy)->EntryInput!=EMythAttackInput::Heavy)))
    {OutError = TEXT("Invalid entry node ID"); return false;}
    for (const FMythComboLink& L : Links)
    {
        const FString Edge = FString::Printf(TEXT("%s/%d"),*L.From.ToString(),static_cast<int32>(L.Input));
        if (!Known.Contains(L.From) || !Known.Contains(L.To) || Edges.Contains(Edge))
        {OutError = TEXT("Invalid/duplicate combo link");return false;}
        Edges.Add(Edge);
    }
    OutError.Reset();return true;
}
