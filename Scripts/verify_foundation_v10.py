#!/usr/bin/env python3
"""Fast source contract verification, NOT an Unreal compiler."""
from pathlib import Path
import sys
ROOT=Path(__file__).resolve().parents[1]
required={
 'MythARPG.uproject':['"MotionWarping"'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Public/MythCampaignTypes.h':['FMythQueuedSignal','FMythTravelRecovery','PendingSignals'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/MythEventSubsystem.cpp':['DeliverPending','StableReceiptId','SetSnapshot(S,true)'],
 'Plugins/MythFoundation/Source/MythFoundationRuntime/Private/MythChapterTravelSubsystem.cpp':['TravelRecovery','bCommittedBeforeTravel'],
 'Plugins/MythGameplay/Source/MythGameplayRuntime/Public/MythExecutionComponent.h':['BeginExecution','CommitImpact','CancelExecution'],
 'Plugins/MythGameplay/Source/MythGameplayRuntime/Public/MythStateTreeCombatTasks.h':['FMythCombatDecisionTask','FMythTacticalMoveTask','GetInstanceDataType'],
 'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythInputBindingSubsystem.cpp':['MapPlayerKey','ResetAllPlayerKeysInRow','SaveSettings'],
 'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythCinematicSubsystem.cpp':['GoToEndAndStop','OnCinematicFinished'],
 'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythQuestSubsystem.cpp':['StableReceiptId','AppliedTransactionIds'],
 'Plugins/MythSystems/Source/MythSystemsRuntime/Private/MythAudioDirectorSubsystem.cpp':['SetSoundMixClassOverride','OnSettingsChanged'],
}
errors=[]
for path,needles in required.items():
 f=ROOT/path
 if not f.is_file(): errors.append('Missing: '+path);continue
 contents=f.read_text(encoding='utf8')
 for needle in needles:
  if needle not in contents:errors.append('Missing '+needle+' in '+path)
 if path.endswith('.cpp') and contents.count('{')!=contents.count('}'):
  errors.append('Unbalanced C++ block braces: '+path)
for p in ROOT.rglob('*'):
 if p.is_file() and p.suffix.lower() in {'.zip','.7z','.tar','.bundle'} and '.git' not in p.parts:
  errors.append('Unexpected archive: '+str(p))
if errors:
 print('\n'.join(errors));sys.exit(1)
print('MythARPG v1.0 source contracts: PASS (NOT Unreal compiled)')
