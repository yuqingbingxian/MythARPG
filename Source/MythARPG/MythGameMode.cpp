#include "MythGameMode.h"
#include "MythPlayerCharacter.h"
AMythGameMode::AMythGameMode()
{
    DefaultPawnClass = AMythPlayerCharacter::StaticClass();
}
