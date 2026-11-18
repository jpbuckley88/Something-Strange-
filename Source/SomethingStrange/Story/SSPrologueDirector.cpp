#include "Story/SSPrologueDirector.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Interaction/SSDoor.h"
#include "Interaction/SSPowerSwitch.h"
#include "Interaction/SSWorkbench.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SSGogglesComponent.h"
#include "Player/SSInventoryComponent.h"
#include "Player/SSPlayerCharacter.h"
#include "SomethingStrange.h"
#include "Story/SSDialogueSubsystem.h"
#include "Story/SSObjectiveSubsystem.h"
#include "Story/SSTutorialTarget.h"
#include "TimerManager.h"
#include "Weapons/SSLaserWeapon.h"

// The lines below are the playable cut of docs/Script.md. Edit them here.
namespace
{
	FSSDialogueLine MomLine(const TCHAR* Text)
	{
		FSSDialogueLine Line;
		Line.Speaker = INVTEXT("MOM");
		Line.Text = FText::FromString(Text);
		return Line;
	}

	FSSDialogueLine NedLine(const TCHAR* Text)
	{
		FSSDialogueLine Line;
		Line.Speaker = INVTEXT("NED");
		Line.Text = FText::FromString(Text);
		return Line;
	}
}

ASSPrologueDirector::ASSPrologueDirector()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASSPrologueDirector::BeginPlay()
{
	Super::BeginPlay();

	// Wait a tick so the player and every placed actor have begun play.
	GetWorldTimerManager().SetTimerForNextTick(this, &ASSPrologueDirector::Begin);
}

void ASSPrologueDirector::Begin()
{
	Ned = Cast<ASSPlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Ned)
	{
		UE_LOG(LogSomethingStrange, Warning, TEXT("%s found no SSPlayerCharacter, so the prologue will not run."), *GetName());
		return;
	}

	// Ned builds his equipment during the prologue, so he starts without it.
	Ned->GetInventory()->RemoveEquipment();

	Ned->GetInventory()->OnInventoryChanged.AddDynamic(this, &ASSPrologueDirector::HandleInventoryChanged);
	Ned->GetGoggles()->OnGogglesToggled.AddDynamic(this, &ASSPrologueDirector::HandleGogglesToggled);
	if (ASSLaserWeapon* Laser = Ned->GetLaser())
	{
		Laser->OnOverheatChanged.AddDynamic(this, &ASSPrologueDirector::HandleOverheatChanged);
	}

	if (FrontDoor)
	{
		FrontDoor->OnDoorToggled.AddDynamic(this, &ASSPrologueDirector::HandleDoorToggled);
	}
	if (LabTrigger)
	{
		LabTrigger->OnActorBeginOverlap.AddDynamic(this, &ASSPrologueDirector::HandleLabTriggerOverlap);
	}
	if (GogglesWorkbench)
	{
		GogglesWorkbench->OnAssembled.AddDynamic(this, &ASSPrologueDirector::HandleAssembled);
	}
	if (LaserWorkbench)
	{
		LaserWorkbench->OnAssembled.AddDynamic(this, &ASSPrologueDirector::HandleAssembled);
	}
	if (PaintCan)
	{
		PaintCan->OnDestroyed.AddDynamic(this, &ASSPrologueDirector::HandlePaintCanDestroyed);
	}
	if (Meat)
	{
		Meat->OnFrequencyTested.AddDynamic(this, &ASSPrologueDirector::HandleFrequencyTested);
	}
	if (Specimen)
	{
		Specimen->OnFrequencyTested.AddDynamic(this, &ASSPrologueDirector::HandleFrequencyTested);
	}
	for (ASSPowerSwitch* Switch : ApertureSwitches)
	{
		if (Switch)
		{
			Switch->OnPowerChanged.AddDynamic(this, &ASSPrologueDirector::HandleSwitchChanged);
		}
	}

	Advance();
}

bool ASSPrologueDirector::IsStepAvailable(EStep InStep) const
{
	switch (InStep)
	{
	case EStep::PhoneCall:
		return FrontDoor != nullptr;
	case EStep::GetShoulder:
		return !ShoulderItemId.IsNone() && !Ned->GetInventory()->HasItem(ShoulderItemId);
	case EStep::GoToLab:
		return LabTrigger != nullptr;
	case EStep::FindParts:
		return GogglesWorkbench && !GogglesWorkbench->RequiredItems.IsEmpty();
	case EStep::AssembleGoggles:
		return GogglesWorkbench != nullptr;
	case EStep::UseGoggles:
	case EStep::GogglesRunOut:
		return Ned->GetInventory()->HasGoggles();
	case EStep::AssembleLaser:
		return LaserWorkbench != nullptr;
	case EStep::FirePaintCan:
		return IsValid(PaintCan) && Ned->GetInventory()->HasLaser();
	case EStep::MaterialOnMeat:
	case EStep::SpectralOnMeat:
		return Meat != nullptr && Ned->GetInventory()->HasLaser();
	case EStep::SpectralOnSpecimen:
		return Specimen != nullptr && Ned->GetInventory()->HasLaser();
	case EStep::StartAperture:
		return ApertureSwitches.ContainsByPredicate([](const ASSPowerSwitch* Switch)
		{
			return Switch && !Switch->IsPowerOn();
		});
	default:
		return true;
	}
}

void ASSPrologueDirector::Advance()
{
	if (Step == EStep::Finished)
	{
		return;
	}

	// Move to the next step that this level is set up for.
	EStep Next = Step;
	do
	{
		Next = static_cast<EStep>(static_cast<uint8>(Next) + 1);
	}
	while (Next != EStep::Finished && !IsStepAvailable(Next));

	EnterStep(Next);
}

void ASSPrologueDirector::EnterStep(EStep NewStep)
{
	Step = NewStep;

	UWorld* World = GetWorld();
	USSObjectiveSubsystem* Objectives = World->GetSubsystem<USSObjectiveSubsystem>();
	USSDialogueSubsystem* Dialogue = World->GetSubsystem<USSDialogueSubsystem>();

	switch (Step)
	{
	case EStep::PhoneCall:
		Objectives->ShowHint(INVTEXT("Move the mouse to look. E to interact."), 12.f);
		Objectives->SetObjective(INVTEXT("Open the front door"));
		Dialogue->PlayLines({
			MomLine(TEXT("...so I'm stuck here till six. Are you home?")),
			NedLine(TEXT("Just got here.")),
			MomLine(TEXT("Okay. Two things. One: there's a pork shoulder in the freezer. Take it out and leave it on the counter.")),
			NedLine(TEXT("Shoulder. Counter. Got it.")),
			MomLine(TEXT("Two. No experiments. Nothing in the basement until I'm home.")),
			NedLine(TEXT("Mom...")),
			MomLine(TEXT("The Hendersons' garage door opened by itself for a week.")),
			NedLine(TEXT("That was a grounding issue, and I fixed it.")),
			MomLine(TEXT("Not. Until. I'm. Home. Say it back.")),
			NedLine(TEXT("Nothing in the basement until you're home.")),
		});
		break;

	case EStep::GetShoulder:
		Objectives->ShowHint(INVTEXT("W A S D to walk."));
		Objectives->SetObjective(INVTEXT("Get the pork shoulder out of the freezer"));
		Dialogue->PlayLines({
			MomLine(TEXT("Thank you. Eat something. Love you.")),
			NedLine(TEXT("Love you. Bye.")),
			NedLine(TEXT("Technically it's already running. That's not starting anything.")),
		});
		break;

	case EStep::GoToLab:
		Objectives->SetObjective(INVTEXT("Go down to the lab"));
		Dialogue->PlayLines({
			NedLine(TEXT("She said take it out. She didn't say where.")),
		});
		break;

	case EStep::FindParts:
		UpdatePartsObjective();
		Dialogue->PlayLines({
			NedLine(TEXT("Still there. Nine hours. You're real, you're stable, and I can't see you. Yet.")),
			NedLine(TEXT("Lens array. Tuning coil. Power cell. Where did I...")),
		});
		break;

	case EStep::AssembleGoggles:
		Objectives->SetObjective(INVTEXT("Assemble the goggles at the workbench"));
		break;

	case EStep::UseGoggles:
		Objectives->ShowHint(INVTEXT("Right mouse to switch the goggles on."), 12.f);
		Objectives->SetObjective(INVTEXT("Look at the specimen jar through the goggles"));
		break;

	case EStep::GogglesRunOut:
		Dialogue->PlayLines({
			NedLine(TEXT("Hi.")),
			NedLine(TEXT("I was right. They said it was sensor noise and I was right.")),
		});
		break;

	case EStep::AssembleLaser:
		Objectives->ShowHint(INVTEXT("The goggles recharge while they are off."));
		Objectives->SetObjective(INVTEXT("Assemble the emitter at the second bench"));
		Dialogue->PlayLines({
			NedLine(TEXT("Five seconds. Okay. Capacitors. That's a tomorrow problem.")),
			NedLine(TEXT("Seeing it is half. If it's energy on a frequency, I can push energy back at it.")),
		});
		break;

	case EStep::FirePaintCan:
		Objectives->ShowHint(INVTEXT("Hold left mouse to fire."), 12.f);
		Objectives->SetObjective(INVTEXT("Test the emitter on the paint can"));
		Dialogue->PlayLines({
			NedLine(TEXT("Resonant emitter, mark four. Marks one to three are why we have a new smoke alarm.")),
		});
		break;

	case EStep::MaterialOnMeat:
		Meat->SetTargetActive(true);
		if (ASSLaserWeapon* Laser = Ned->GetLaser())
		{
			Laser->SetFrequency(ESSFrequency::Material);
		}
		Objectives->SetObjective(INVTEXT("Test the material frequency on the meat"));
		Dialogue->PlayLines({
			NedLine(TEXT("Two settings. Material: things that are properly here. Spectral: things that aren't. Yet.")),
		});
		break;

	case EStep::SpectralOnMeat:
		Objectives->ShowHint(INVTEXT("Q to switch frequency."), 12.f);
		Objectives->SetObjective(INVTEXT("Test the spectral frequency on the meat"));
		Dialogue->PlayLines({
			NedLine(TEXT("Okay. So don't point that at anything I like.")),
			NedLine(TEXT("...It's thawed.")),
		});
		break;

	case EStep::SpectralOnSpecimen:
		Objectives->ShowHint(INVTEXT("The specimen only shows through the goggles."), 12.f);
		Objectives->SetObjective(INVTEXT("Test the spectral frequency on the specimen"));
		Dialogue->PlayLines({
			NedLine(TEXT("Nothing. Passes straight through. Good.")),
		});
		break;

	case EStep::StartAperture:
		Objectives->SetObjective(INVTEXT("Start the aperture"));
		Dialogue->PlayLines({
			NedLine(TEXT("Sorry. Sorry! Okay. That hurt you. That's... huh.")),
			NedLine(TEXT("That's not what I built it for.")),
			MomLine(TEXT("Ned? I'm home! Did you get the shoulder out?")),
			NedLine(TEXT("...It's out!")),
			MomLine(TEXT("I don't see it on the counter.")),
			NedLine(TEXT("It's... thawing down here! Where it's warmer!")),
			NedLine(TEXT("If I can open a window, a small one, for one second, and look through...")),
		});
		break;

	case EStep::Finished:
		Objectives->ClearObjective();
		Dialogue->StopAll();
		Dialogue->PlayLines({
			MomLine(TEXT("Ned, the lights are doing the thing again. What did I say?")),
			NedLine(TEXT("You're home! You said until you're home! One second! Literally one second!")),
		});
		OnPrologueFinished.Broadcast();
		break;

	default:
		break;
	}
}

void ASSPrologueDirector::UpdatePartsObjective()
{
	const TArray<FName>& Parts = GogglesWorkbench->RequiredItems;
	const int32 Found = Ned->GetInventory()->CountItems(Parts);

	GetWorld()->GetSubsystem<USSObjectiveSubsystem>()->SetObjective(FText::Format(
		INVTEXT("Find the missing parts for the goggles ({0} of {1})"), FText::AsNumber(Found), FText::AsNumber(Parts.Num())));
}

void ASSPrologueDirector::HandleDoorToggled(bool bOpen)
{
	if (Step == EStep::PhoneCall && bOpen)
	{
		Advance();
	}
}

void ASSPrologueDirector::HandleInventoryChanged()
{
	const USSInventoryComponent* Inventory = Ned->GetInventory();

	if (Step == EStep::GetShoulder && Inventory->HasItem(ShoulderItemId))
	{
		Advance();
	}
	else if (Step == EStep::FindParts)
	{
		const TArray<FName>& Parts = GogglesWorkbench->RequiredItems;
		if (Inventory->CountItems(Parts) >= Parts.Num())
		{
			Advance();
		}
		else
		{
			UpdatePartsObjective();
		}
	}
}

void ASSPrologueDirector::HandleLabTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Step == EStep::GoToLab && OtherActor == Ned.Get())
	{
		Advance();
	}
}

void ASSPrologueDirector::HandleAssembled(ASSWorkbench* Workbench)
{
	if ((Step == EStep::AssembleGoggles && Workbench == GogglesWorkbench)
		|| (Step == EStep::AssembleLaser && Workbench == LaserWorkbench))
	{
		Advance();
	}
}

void ASSPrologueDirector::HandleGogglesToggled(bool bActive)
{
	if ((Step == EStep::UseGoggles && bActive) || (Step == EStep::GogglesRunOut && !bActive))
	{
		Advance();
	}
}

void ASSPrologueDirector::HandleOverheatChanged(bool bOverheated)
{
	if (bOverheated && !bShownOverheatHint && Step != EStep::Finished)
	{
		bShownOverheatHint = true;
		GetWorld()->GetSubsystem<USSObjectiveSubsystem>()->ShowHint(INVTEXT("The emitter overheats. Let it cool."));
		GetWorld()->GetSubsystem<USSDialogueSubsystem>()->PlayLines({
			NedLine(TEXT("Don't melt it. Took me three weeks to find that coil.")),
		});
	}
}

void ASSPrologueDirector::HandlePaintCanDestroyed(AActor* DestroyedActor)
{
	if (Step == EStep::FirePaintCan)
	{
		Advance();
	}
}

void ASSPrologueDirector::HandleFrequencyTested(ASSTutorialTarget* Target, ESSFrequency Frequency)
{
	const bool bMeat = Target == Meat;
	const bool bSpecimen = Target == Specimen;

	if ((Step == EStep::MaterialOnMeat && bMeat && Frequency == ESSFrequency::Material)
		|| (Step == EStep::SpectralOnMeat && bMeat && Frequency == ESSFrequency::Spectral)
		|| (Step == EStep::SpectralOnSpecimen && bSpecimen && Frequency == ESSFrequency::Spectral))
	{
		Advance();
	}
}

void ASSPrologueDirector::HandleSwitchChanged(bool bPowerOn)
{
	if (Step != EStep::StartAperture)
	{
		return;
	}

	const bool bAllOn = !ApertureSwitches.ContainsByPredicate([](const ASSPowerSwitch* Switch)
	{
		return Switch && !Switch->IsPowerOn();
	});

	if (bAllOn)
	{
		Advance();
	}
}
