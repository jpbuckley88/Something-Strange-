#pragma once

#include "CoreMinimal.h"
#include "Entities/SSEntityCharacter.h"
#include "SSWisp.generated.h"

/**
 * The weakest entity: small, fast, flying, usually in groups. Barely merged.
 *
 * - Maw (orange): the only part that has crossed over, and what it bites with.
 *   Destroy it with the material beam and the wisp is harmless.
 * - Core (blue): seen only through the goggles. Destroy it with the spectral
 *   beam to banish the wisp.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSWisp : public ASSEntityCharacter
{
	GENERATED_BODY()

public:
	ASSWisp();
};
