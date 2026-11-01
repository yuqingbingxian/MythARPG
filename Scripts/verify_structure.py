#!/usr/bin/env python3
"""No-engine source structural audit. DOES NOT compile Unreal C++."""
from pathlib import Path
import json,re,sys
root=Path(__file__).resolve().parents[1]
game=root/'Plugins/MythGameplay/Source/MythGameplayRuntime'
required=[
'Public/MythActionComponent.h','Private/MythActionComponent.cpp',
'Public/MythComboGraph.h','Private/MythComboGraph.cpp',
'Public/MythWeaponTraceComponent.h','Private/MythWeaponTraceComponent.cpp',
'Public/MythLockOnComponent.h','Private/MythLockOnComponent.cpp',
'Public/MythAnimNotifyState_HitWindow.h','Public/MythAnimNotifyState_ComboWindow.h',
'Public/MythAnimNotifyState_IFrameWindow.h','Public/MythAnimNotifyState_DodgeCancelWindow.h',
'Private/Tests/MythComboGraphTest.cpp'
]
errors=[]
for path in required:
    if not (game/path).is_file():errors.append('Missing: '+path)
for h in game.rglob('*.h'):
    source=h.read_text()
    if 'UCLASS(' in source or 'USTRUCT(' in source or 'UENUM(' in source:
        if f'#include "{h.stem}.generated.h"' not in source:errors.append('UHT include missing '+str(h))
        if source.find(f'#include "{h.stem}.generated.h"') < 0: continue
        after=source.split(f'#include "{h.stem}.generated.h"',1)[-1]
        if re.search(r'^\s*#include',after,re.MULTILINE):errors.append('Include after generated.h: '+str(h))
project=json.loads((root/'MythARPG.uproject').read_text())
if project.get('EngineAssociation')!='5.8':errors.append('Unexpected EngineAssociation')
mods=[x['Name'] for x in project['Plugins'] if x.get('Enabled')]
for mod in ['MythGameplay','MythFoundation','GameplayAbilities','EnhancedInput']:
    if mod not in mods:errors.append('Plugin not enabled '+mod)
tags=(root/'Config/DefaultGameplayTags.ini').read_text()
for tag in ['State.Combat.Attacking','State.Movement.Dodging','State.Combat.Staggered','State.Defense.Invulnerable','State.Character.Dead']:
    if tag not in tags:errors.append('Missing tag '+tag)
strings={'MythActionComponent.cpp':['BeginAttack','TryAdvance','OpenHitWindow','AbortDodge'],
'MythCombatCharacter.cpp':['ApplyMeleeHit','OnPoiseAttributeChanged'],
'MythPlayerCharacter.cpp':['HeavyAttack','ToggleLockOn']}
for file,items in strings.items():
    src=(game/'Private'/file).read_text()
    for item in items:
        if item not in src:errors.append('Missing method '+item+' in '+file)
print('v0.2 structure audit:', 'PASS' if not errors else 'FAIL')
print('C++ files:',len(list(game.rglob('*.cpp'))),' headers:',len(list(game.rglob('*.h'))))
print('Important: no Unreal compile/UHT/PIE validation has been run.')
for e in errors:print('ERROR:',e)
sys.exit(1 if errors else 0)
