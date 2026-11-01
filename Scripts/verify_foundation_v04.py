#!/usr/bin/env python3
"""Static, offline smoke assertions; NOT a C++ compile or UE runtime test."""
from pathlib import Path
import json
root = Path(__file__).resolve().parents[1]
proj = json.loads((root/'MythARPG.uproject').read_text(encoding='utf-8'))
enabled = {p['Name'] for p in proj['Plugins'] if p.get('Enabled')}
assert 'MythEditor' in enabled
check = {
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Public/MythSaveStorage.h': ['ReadBest', 'WriteNext', 'Unwrap'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/MythSaveStorage.cpp': ['RecoveredOlderGeneration', 'FCrc::MemCrc32', 'Campaign_00'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Public/MythChapterTravelSubsystem.h': ['EMythTravelPhase', 'RequestChapterTravel', 'CancelPendingTravel'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/MythChapterTravelSubsystem.cpp': ['PostLoadMapWithWorld', 'SelectChapter', 'OpenLevel'],
 'Plugins/MythEditor/Source/MythEditorTools/Private/MythContentAuditLibrary.cpp': ['AuditAllGameAssets', 'AuditWorldPersistentIds', 'ValidateDefinition'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/Tests/MythSaveStorageTest.cpp': ['Corruption injection', 'Previous generation'],
}
for path, tokens in check.items():
    content = (root/path).read_text(encoding='utf-8')
    for token in tokens:
        assert token in content, f'{path}: missing {token}'
for path in root.rglob('*'):
    if path.is_file():
        assert path.suffix.lower() not in {'.zip','.7z','.rar','.bundle'}
print(f'v0.4 foundation static audit: PASS ({len(check)} critical implementation files)')
print('Engine compilation, runtime/map transition, filesystem crash recovery: NOT VERIFIED')
