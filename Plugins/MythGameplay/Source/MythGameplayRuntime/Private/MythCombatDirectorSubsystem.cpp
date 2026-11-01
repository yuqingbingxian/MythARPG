#include "MythCombatDirectorSubsystem.h"
#include "Engine/World.h"
void UMythCombatDirectorSubsystem::RemoveExpired()
{
    const double Now=GetWorld()?GetWorld()->GetTimeSeconds():0.0;
    Tickets.RemoveAll([Now](const FMythAttackTicket& T)
    {return !T.Attacker.IsValid() || !T.Target.IsValid() || T.ExpiresAt<=Now;});
}
bool UMythCombatDirectorSubsystem::TryReserveAttack(AActor* Attacker,AActor* Target,float HoldSeconds)
{
    if(!IsValid(Attacker) || !IsValid(Target) || Attacker==Target)return false;
    RemoveExpired();
    for(FMythAttackTicket& Ticket:Tickets)
    {
        if(Ticket.Attacker.Get()==Attacker)
        {
            // One attacker cannot reserve multiple slots.
            if(Ticket.Target.Get()!=Target)return false;
            Ticket.ExpiresAt=GetWorld()->GetTimeSeconds()+FMath::Max(.1f,HoldSeconds);
            return true;
        }
    }
    int32 Active=0;
    for(const FMythAttackTicket& Ticket:Tickets)if(Ticket.Target.Get()==Target)++Active;
    if(Active>=MaxAttackersPerTarget)return false;
    FMythAttackTicket Ticket;Ticket.Attacker=Attacker;Ticket.Target=Target;
    Ticket.ExpiresAt=GetWorld()->GetTimeSeconds()+FMath::Max(.1f,HoldSeconds);
    Tickets.Add(Ticket);
    return true;
}
void UMythCombatDirectorSubsystem::ReleaseAttack(AActor* Attacker)
{
    Tickets.RemoveAll([Attacker](const FMythAttackTicket& T){return T.Attacker.Get()==Attacker;});
}
