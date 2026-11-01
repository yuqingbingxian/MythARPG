#!/usr/bin/env python3
"""Static v0.3 source checks. Does not compile Unreal or validate gameplay at runtime."""
from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
uproject=json.loads((root/'MythARPG.uproject').read_text(encoding='utf8'))
plugins={p['Name'] for p in uproject.get('Plugins',[]) if p.get('Enabled',False)}
assert {'MythFoundation','MythGameplay','MythSystems'} <= plugins, plugins
assert 'MythSystemsRuntime' in (root/'Source/MythARPG/MythARPG.Build.cs').read_text()
headers=list(root.glob('Plugins/**/Public/*.h'))
assert len(headers)>=25, f'expected >=25 headers, got {len(headers)}'
for path in headers:
    text=path.read_text(encoding='utf8')
    if 'UCLASS(' in text or 'USTRUCT(' in text or 'UENUM(' in text or 'UINTERFACE(' in text:
        declaration=re.search(r'#include\s+"([^"\n]+\.generated\.h)"',text)
        assert declaration, f'missing generated header: {path}'
        assert declaration.group(1)==path.stem+'.generated.h',path
    assert len(text)>40,path
for required in (
    'MythSaveSubsystem','MythWorldStateSubsystem','MythChapterSubsystem',
    'MythInventorySubsystem','MythQuestSubsystem','MythNarrativeSubsystem',
    'MythCheckpointSubsystem','MythCombatDirectorSubsystem','MythBossPhaseDefinition'):
    assert any(p.stem==required for p in headers), required
for path in root.rglob('*.zip'):
    raise AssertionError(f'archive in source directory: {path}')
print(f'MythARPG v0.3 static structure: PASS ({len(headers)} public headers)')
print('NOT VERIFIED: UnrealBuildTool/UHT, PIE, gameplay correctness, save migration in UE.')
