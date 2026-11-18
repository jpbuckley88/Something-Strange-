#include "Core/SSGameMode.h"

#include "Player/SSPlayerCharacter.h"
#include "UI/SSHUD.h"

ASSGameMode::ASSGameMode()
{
	DefaultPawnClass = ASSPlayerCharacter::StaticClass();
	HUDClass = ASSHUD::StaticClass();
}
