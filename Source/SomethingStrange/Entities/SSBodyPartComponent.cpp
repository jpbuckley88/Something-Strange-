#include "Entities/SSBodyPartComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Spectral/SSSpectralSubsystem.h"

namespace
{
	const FLinearColor MergedTint(1.f, 0.35f, 0.05f);
	const FLinearColor UnmergedTint(0.1f, 0.9f, 1.f);
}

USSBodyPartComponent::USSBodyPartComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USSBodyPartComponent::BeginPlay()
{
	Super::BeginPlay();

	for (FSSBodyPartState& Part : Parts)
	{
		Part.Health = Part.MaxHealth;
		Part.bDestroyed = false;
	}

	if (USSSpectralSubsystem* Spectral = GetWorld()->GetSubsystem<USSSpectralSubsystem>())
	{
		Spectral->OnSpectralVisionChanged.AddDynamic(this, &USSBodyPartComponent::HandleSpectralVisionChanged);
		bSpectralVisionActive = Spectral->IsSpectralVisionActive();
	}

	RefreshPartVisibility();
}

void USSBodyPartComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (USSSpectralSubsystem* Spectral = World->GetSubsystem<USSSpectralSubsystem>())
		{
			Spectral->OnSpectralVisionChanged.RemoveDynamic(this, &USSBodyPartComponent::HandleSpectralVisionChanged);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void USSBodyPartComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bBanished)
	{
		return;
	}

	// Entities keep crossing over: parts with a merge rate drift towards fully merged.
	bool bAnyCrossed = false;
	for (FSSBodyPartState& Part : Parts)
	{
		if (Part.bDestroyed || Part.MergeRate <= 0.f || Part.MergeAmount >= 1.f)
		{
			continue;
		}

		const bool bWasMerged = Part.IsMerged();
		Part.MergeAmount = FMath::Min(1.f, Part.MergeAmount + Part.MergeRate * DeltaTime);

		if (Part.IsMerged() != bWasMerged)
		{
			bAnyCrossed = true;
			OnPartMergeChanged.Broadcast(Part.PartName, Part.IsMerged());
		}
	}

	if (bAnyCrossed)
	{
		RefreshPartVisibility();
	}
}

int32 USSBodyPartComponent::FindPartIndexFromHit(const FHitResult& Hit) const
{
	const UPrimitiveComponent* HitComponent = Hit.GetComponent();
	if (!HitComponent)
	{
		return INDEX_NONE;
	}

	// Component tags: one primitive component per part.
	for (int32 Index = 0; Index < Parts.Num(); ++Index)
	{
		const FSSBodyPartState& Part = Parts[Index];
		if (!Part.bDestroyed && !Part.ComponentTag.IsNone() && HitComponent->ComponentHasTag(Part.ComponentTag))
		{
			return Index;
		}
	}

	// Bones: walk up from the hit bone until one belongs to a part.
	const USkinnedMeshComponent* SkinnedMesh = Cast<USkinnedMeshComponent>(HitComponent);
	FName Bone = Hit.BoneName;
	while (!Bone.IsNone())
	{
		for (int32 Index = 0; Index < Parts.Num(); ++Index)
		{
			const FSSBodyPartState& Part = Parts[Index];
			if (!Part.bDestroyed && Part.Bones.Contains(Bone))
			{
				return Index;
			}
		}

		Bone = SkinnedMesh ? SkinnedMesh->GetParentBone(Bone) : NAME_None;
	}

	return INDEX_NONE;
}

int32 USSBodyPartComponent::FindPartIndexByName(FName PartName) const
{
	return Parts.IndexOfByPredicate([PartName](const FSSBodyPartState& Part)
	{
		return Part.PartName == PartName;
	});
}

float USSBodyPartComponent::ApplyBeamDamage(const FHitResult& Hit, ESSFrequency Frequency, float BaseDamage)
{
	if (bBanished || BaseDamage <= 0.f)
	{
		return 0.f;
	}

	const int32 PartIndex = FindPartIndexFromHit(Hit);
	if (PartIndex == INDEX_NONE)
	{
		return 0.f;
	}

	FSSBodyPartState& Part = Parts[PartIndex];
	const bool bMatched = Part.GetMatchingFrequency() == Frequency;
	const float Damage = BaseDamage * (bMatched ? MatchedDamageMultiplier : MismatchedDamageMultiplier);

	if (!Part.bIndestructible)
	{
		Part.Health = FMath::Max(0.f, Part.Health - Damage);
	}
	const bool bDestroyed = Part.Health <= 0.f;

	// Part is not used after this: a listener may change the Parts array.
	OnPartDamaged.Broadcast(Part.PartName, Frequency, bMatched, BaseDamage);

	if (bDestroyed)
	{
		DestroyPart(PartIndex);
	}

	return Damage;
}

void USSBodyPartComponent::DestroyPart(int32 PartIndex)
{
	FSSBodyPartState& Part = Parts[PartIndex];
	Part.bDestroyed = true;

	// Copy what the broadcasts need: a listener may modify Parts or destroy the owner.
	const FName PartName = Part.PartName;
	const bool bWasCore = Part.bIsCore;

	RefreshPartVisibility();
	OnPartDestroyed.Broadcast(PartName);

	if (bWasCore)
	{
		bBanished = true;
		OnBanished.Broadcast();

		if (bDestroyOwnerOnBanish && GetOwner())
		{
			GetOwner()->Destroy();
		}
	}
}

void USSBodyPartComponent::HandleSpectralVisionChanged(bool bActive)
{
	bSpectralVisionActive = bActive;
	RefreshPartVisibility();
}

void USSBodyPartComponent::RefreshPartVisibility()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	for (const FSSBodyPartState& Part : Parts)
	{
		if (Part.ComponentTag.IsNone())
		{
			continue;
		}

		const bool bVisible = !Part.bDestroyed && (Part.IsMerged() || bSpectralVisionActive);

		for (UActorComponent* Component : OwnerActor->GetComponentsByTag(UPrimitiveComponent::StaticClass(), Part.ComponentTag))
		{
			UPrimitiveComponent* Primitive = CastChecked<UPrimitiveComponent>(Component);
			Primitive->SetVisibility(bVisible);

			if (bTintPlaceholderMeshes)
			{
				// The engine's basic shape material exposes a "Color" parameter.
				if (UMaterialInstanceDynamic* Material = Primitive->CreateDynamicMaterialInstance(0))
				{
					Material->SetVectorParameterValue(TEXT("Color"), Part.IsMerged() ? MergedTint : UnmergedTint);
				}
			}

			if (Part.bDestroyed)
			{
				Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}
