#!/usr/bin/env python3
"""Offline code-contract inspection only (not a UE compile)."""
from pathlib import Path
import sys,json
r=Path(__file__).resolve().parents[1]
errors=[]
h=(r/'Plugins/MythGameplay/Source/MythGameplayRuntime/Public/MythEnemyAIController.h').read_text()
cpp=(r/'Plugins/MythGameplay/Source/MythGameplayRuntime/Private/MythEnemyAIController.cpp').read_text()
build=(r/'Plugins/MythGameplay/Source/MythGameplayRuntime/MythGameplayRuntime.Build.cs').read_text()
plugin=json.loads((r/'Plugins/MythGameplay/MythGameplay.uplugin').read_text())
for t in ('UStateTreeAIComponent','bUseStateTree','RequestTacticalPosition','OnTacticalQueryFinished','GetTacticalPosition'):
    if t not in h:errors.append('AI controller header missing '+t)
for t in ('StateTreeBrain->StartLogic','StateTreeBrain->StopLogic','Request.Execute(EEnvQueryRunMode::SingleResult','Result->QueryID!=PendingQueryId','SilentlyRemoveAllQueriesByQuerier','GetItemAsLocation'):
    if t not in cpp:errors.append('AI adapter source missing '+t)
if 'GameplayStateTreeModule' not in build:errors.append('Missing GameplayStateTreeModule Build.cs')
if 'GameplayStateTree' not in [p['Name'] for p in plugin['Plugins']]:errors.append('GameplayStateTree plugin not enabled')
if 'Docs/15_AI_STATETREE_EQS_V08.md' not in [f.relative_to(r).as_posix() for f in r.rglob('*.md')]: errors.append('Missing guide')
print('MythARPG v0.8 source-contract audit:', 'FAIL' if errors else 'PASS')
print('Unreal compilation/StateTree asset/EQS runtime still NOT TESTED')
for e in errors:print('ERROR',e)
sys.exit(bool(errors))
