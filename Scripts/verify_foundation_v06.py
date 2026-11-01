#!/usr/bin/env python3
"""Structural checks only; NOT Unreal compilation."""
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
tokens={
    'Plugins/MythSystems/Source/MythSystemsRuntime/Public/MythEncounterDefinition.h':['UPrimaryDataAsset','Waves','EncounterId'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythEncounterDirector.cpp':['StartWave','OnEnemyDeath','FinishEncounter','TryGrantToSnapshot','WorldValues'],
    'Plugins/MythEditor/Source/MythEditorTools/Private/MythContentAuditLibrary.cpp':['UMythEncounterDefinition::StaticClass','Encounter->Validate'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Private/Tests/MythEncounterDefinitionTest.cpp':['Encounter.Validation'],
}
errors=[]
for filename,required in tokens.items():
 p=root/filename
 if not p.exists():errors.append('Missing: '+filename);continue
 content=p.read_text()
 for tok in required:
  if tok not in content:errors.append(filename+' missing '+tok)
print('MythARPG v0.6 encounter static audit:', 'FAIL' if errors else 'PASS')
for e in errors:print(e)
sys.exit(bool(errors))
