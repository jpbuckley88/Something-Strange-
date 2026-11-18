#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSRiftAnchor.generated.h"

class USSBodyPartComponent;
class UStaticMeshComponent;

/**
 * One of the points holding a rift open. Seen only through the goggles and
 * destroyed with the spectral beam. Assign anchors to a rift in the level.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSRiftAnchor : public AActor
{
	GENERATED_BODY()

public:
	ASSRiftAnchor();

private:
	UPROPERTY(VisibleAnywhere, Category = "Rift")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Rift")
	TObjectPtr<USSBodyPartComponent> BodyParts;
};
