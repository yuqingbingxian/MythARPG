#!/usr/bin/env python3
"""Static no-engine source/contract audit. Not an Unreal compiler or UHT."""
from pathlib import Path
import json,re,sys
root=Path(__file__).resolve().parents[1]
errors=[]
for path, tokens in {
    'Plugins/MythGameplay/Source/MythGameplayRuntime/Public/MythDefenseComponent.h':['ResolveIncoming','OpenParryWindow','ResetDefense'],
    'Plugins/MythGameplay/Source/MythGameplayRuntime/Private/MythCombatCharacter.cpp':['ResolveIncoming','EMythDefenseOutcome::Parried'],
    'Plugins/MythGameplay/Source/MythGameplayRuntime/Private/MythEnemyAIController.cpp':['OnTargetStimulus','TryReserveAttack','ChooseAttack'],
    'Plugins/MythGameplay/Source/MythGameplayRuntime/Public/MythAICombatProfile.h':['ChooseAttack','FMythAIAttackOption'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Public/MythQuestDefinition.h':['FMythQuestBranch','ResolveNextStage'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythNarrativeSubsystem.cpp':['ApplyToSnapshot','PublishEvents'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Public/MythUIRouterSubsystem.h':['PushWidget','PopWidget'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Public/MythHUDViewModel.h':['BindCombatant','HealthFraction'],
    'Plugins/MythSystems/Source/MythSystemsRuntime/Public/MythSettingsSubsystem.h':['SetAccessibility','SetAudioLevels'],
    'Plugins/MythEditor/Source/MythEditorTools/Private/MythContentAuditLibrary.cpp':['UMythNarrativeCommandDefinition','UMythAICombatProfile'],
}.items():
    p=root/path
    if not p.is_file():errors.append('Missing '+path);continue
    for token in tokens:
        if token not in p.read_text(encoding='utf8'):errors.append(path+' missing '+token)
for h in root.rglob('*.h'):
    if 'Plugins' not in h.parts:continue
    src=h.read_text(encoding='utf8')
    if any(t in src for t in ('UCLASS(', 'USTRUCT(', 'UENUM(')):
        name='#include "'+h.stem+'.generated.h"'
        if name not in src:errors.append('Missing UHT include '+str(h))
        elif re.search(r'^#include ',src.split(name,1)[1],re.M):errors.append('Include after generated header '+str(h))
for plugin in ('MythGameplay','MythSystems','MythEditor'):
    p=root/'Plugins'/plugin/(plugin+'.uplugin')
    if not p.exists():errors.append('Missing plugin '+plugin)
    else:json.loads(p.read_text())
cs=(root/'Plugins/MythEditor/Source/MythEditorTools/MythEditorTools.Build.cs').read_text()
if 'MythGameplayRuntime' not in cs:errors.append('Editor missing gameplay dependency')
cs=(root/'Plugins/MythSystems/Source/MythSystemsRuntime/MythSystemsRuntime.Build.cs').read_text()
if 'UMG' not in cs:errors.append('Systems missing UMG dependency')
for tag in ('State.Defense.Guarding','State.Defense.ParryWindow'):
    if tag not in (root/'Config/DefaultGameplayTags.ini').read_text():errors.append('Missing '+tag)
for f in root.rglob('*'):
    if f.is_file() and f.suffix.lower() in ('.zip','.7z','.tar','.bundle'):
        errors.append('Binary archive in project: '+str(f))
print('MythARPG v0.5 static audit:', 'FAIL' if errors else 'PASS')
print('C++ sources:',len(list(root.rglob('*.cpp'))),' C++ headers:',len(list(root.rglob('*.h'))))
print('Note: UnrealBuildTool/UHT/PIE/runtime not executed.')
for e in errors:print('ERROR:',e)
sys.exit(bool(errors))
