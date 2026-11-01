#!/usr/bin/env python3
"""Conservative duplicate-definition / duplicate-property scan, not a C++ compiler.
This does not parse templates, overload sets, macros or full classes.
"""
from pathlib import Path
from collections import Counter
import re,sys
root=Path(__file__).resolve().parents[1]
issues=[]
# Detect repeated same fully qualified function and arguments (not overloads)
method=re.compile(r'^\s*(?:[\w:<>,*& ]+?\s+)?([A-Za-z_]\w*::(?:~?[A-Za-z_]\w*))\s*\(([^)]*)\)\s*(?:const)?\s*(?:\{|$)',re.M)
for cpp in root.rglob('*.cpp'):
    names=[(n, re.sub(r'\s+','',args)) for n,args in method.findall(cpp.read_text(encoding='utf8'))]
    for key,count in Counter(names).items():
        if count>1:issues.append(f'{cpp.relative_to(root)}: repeated method {key} ({count})')
# Same exact member property (within a single header) should never be repeated
prop=re.compile(r'\b(?:TObjectPtr<[^>]+>|TSoftObjectPtr<[^>]+>|TSubclassOf<[^>]+>|(?:int32|int64|float|double|bool|FName|FString))\s+([A-Za-z_]\w*)\s*(?:[;=])')
for header in root.rglob('*.h'):
    txt=header.read_text(encoding='utf8')
    # Limit member detection to UPROPERTY-heavy type headers, avoid local variables outside a class.
    if 'UCLASS(' not in txt and 'USTRUCT(' not in txt:continue
    for block in re.split(r'(?=^\s*(?:UCLASS|USTRUCT)\()',txt,flags=re.M):
        if not ('UCLASS(' in block or 'USTRUCT(' in block):continue
        names=[]
        for line in block.splitlines():
            if 'UPROPERTY(' in line or 'TObjectPtr<' in line:
                names.extend(prop.findall(line))
        for name,count in Counter(names).items():
            if count>1:issues.append(f'{header.relative_to(root)}: duplicate reflected property {name} ({count})')
# Guard against duplicated input binding lines, which can cause duplicated actions
player=(root/'Plugins/MythGameplay/Source/MythGameplayRuntime/Private/MythPlayerCharacter.cpp').read_text()
for token in ('E->BindAction(GuardAction,ETriggerEvent::Started','E->BindAction(ParryAction,ETriggerEvent::Started'):
    if player.count(token)!=1:issues.append(f'Input binding duplicated or missing: {token}')
print('MythARPG v0.9 duplicate-code scan:', 'FAIL' if issues else 'PASS')
for issue in issues:print(issue)
print('WARNING: heuristics, not a UE/Clang compile')
sys.exit(bool(issues))
