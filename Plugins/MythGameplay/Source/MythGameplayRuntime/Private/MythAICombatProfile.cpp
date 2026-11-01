#include "MythAICombatProfile.h"
const FMythAIAttackOption* UMythAICombatProfile::ChooseAttack(float Distance,
    const TMap<FName,double>& LastUsed,double Now,FRandomStream& Random) const
{
    float Total=0.f;
    for(const FMythAIAttackOption& Attack:Attacks)
        if(!Attack.AttackId.IsNone() && Attack.Weight>0.f && Distance>=Attack.MinDistance &&
           Distance<=Attack.MaxDistance &&
           (!LastUsed.Contains(Attack.AttackId) || Now-LastUsed.FindRef(Attack.AttackId)>=Attack.Cooldown))
            Total+=Attack.Weight;
    if(Total<=0.f)return nullptr;
    float Roll=Random.FRandRange(0.f,Total);
    for(const FMythAIAttackOption& Attack:Attacks)
    {
        if(Attack.AttackId.IsNone() || Attack.Weight<=0.f || Distance<Attack.MinDistance ||
            Distance>Attack.MaxDistance ||
            (LastUsed.Contains(Attack.AttackId) && Now-LastUsed.FindRef(Attack.AttackId)<Attack.Cooldown))continue;
        Roll-=Attack.Weight;
        if(Roll<=0.f)return &Attack;
    }
    return nullptr;
}
