#include "MythGameFlowSubsystem.h"
bool UMythGameFlowSubsystem::TryChangeScreenState(EMythGameScreenState Next)
{
    if(Next==ScreenState)return true;
    // Loading cannot be interrupted by the UI. Only loading-complete may switch to gameplay or menu.
    if(ScreenState==EMythGameScreenState::Loading && Next!=EMythGameScreenState::Exploring && Next!=EMythGameScreenState::MainMenu)return false;
    if(ScreenState==EMythGameScreenState::Dead && Next==EMythGameScreenState::Dialogue)return false;
    EMythGameScreenState Old=ScreenState;
    ScreenState=Next;
    OnScreenStateChanged.Broadcast(Old,Next);
    return true;
}
