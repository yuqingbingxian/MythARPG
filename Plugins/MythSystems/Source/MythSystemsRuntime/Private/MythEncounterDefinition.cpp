#include "MythEncounterDefinition.h"
bool UMythEncounterDefinition::Validate(FString& Error)const
{
    if(EncounterId.IsNone() || Waves.IsEmpty()){Error=TEXT("Encounter needs stable ID and at least one wave");return false;}
    TSet<FName> Seen;
    for(const FMythEncounterWave& Wave:Waves)
    {
        if(Wave.WaveId.IsNone() || Seen.Contains(Wave.WaveId) || Wave.Spawns.IsEmpty())
        {Error=TEXT("Duplicate/empty wave or missing spawn entries");return false;}
        Seen.Add(Wave.WaveId);
        for(const FMythEncounterSpawn& Entry:Wave.Spawns)
            if(Entry.EnemyClass.IsNull() || Entry.Count<1 || Entry.Count>32)
            {Error=TEXT("Invalid spawn class/count");return false;}
    }
    Error.Reset();return true;
}
