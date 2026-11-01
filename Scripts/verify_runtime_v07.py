#!/usr/bin/env python3
"""Static checks for v0.7 integration. Does NOT compile or execute Unreal gameplay."""
from pathlib import Path
import json, re, sys
root = Path(__file__).resolve().parents[1]
errors = []
required = {
    'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythEncounterDirector.cpp':
        ('RequestAsyncLoad', 'OnEncounterClassesReady', 'AssetPreload', 'Entry.EnemyClass.Get()', 'AbortEncounter'),
    'Plugins/MythSystems/Source/MythSystemsRuntime/Public/MythEncounterDirector.h':
        ('Preparing', 'TSharedPtr<FStreamableHandle>', 'OnEncounterClassesReady'),
    'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythUIRouterSubsystem.cpp':
        ('PreLoadMap.AddUObject', 'PreLoadMap.Remove', 'CloseAll()', 'EMythUILayer::Cinematic'),
    'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/MythSaveSubsystem.cpp':
        ('ObjectiveCounts', 'AppliedTransactionIds', 'ValidateSnapshot', 'ChapterId.IsNone()'),
    'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/Tests/MythSnapshotValidationTest.cpp':
        ('IMPLEMENT_SIMPLE_AUTOMATION_TEST', 'TestFalse', 'TestTrue'),
}
for rel, needles in required.items():
    path = root / rel
    if not path.is_file():
        errors.append('missing file: ' + rel)
        continue
    content = path.read_text(encoding='utf8')
    for token in needles:
        if token not in content:
            errors.append('missing token: ' + rel + '::' + token)
# No synchronous class loading in the performance-sensitive combat wave.
encounter = (root / 'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythEncounterDirector.cpp').read_text()
if 'EnemyClass.LoadSynchronous' in encounter:
    errors.append('combat encounter still performs blocking class loading')
for path in root.rglob('*'):
    if path.is_file() and path.suffix.lower() in {'.zip', '.rar', '.7z', '.bundle', '.tar'}:
        errors.append('binary archive in project: ' + str(path))
for up in root.rglob('*.uplugin'):
    try: json.loads(up.read_text())
    except Exception as ex: errors.append(f'bad plugin config {up}: {ex}')
print('MythARPG v0.7 static source audit:', 'FAIL' if errors else 'PASS')
print('Engine compile/UHT/PIE/end-to-end performance: NOT VERIFIED')
for e in errors: print('ERROR:',e)
sys.exit(1 if errors else 0)
