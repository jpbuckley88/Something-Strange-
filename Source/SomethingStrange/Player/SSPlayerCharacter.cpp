#include "Player/SSPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Interaction/SSInteractionComponent.h"
#include "Player/SSGogglesComponent.h"
#include "Weapons/SSLaserWeapon.h"

ASSPlayerCharacter::ASSPlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(40.f, 88.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	Camera->bUsePawnControlRotation = true;

	Goggles = CreateDefaultSubobject<USSGogglesComponent>(TEXT("Goggles"));
	Interaction = CreateDefaultSubobject<USSInteractionComponent>(TEXT("Interaction"));

	LaserClass = ASSLaserWeapon::StaticClass();
}

void ASSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (LaserClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Laser = GetWorld()->SpawnActor<ASSLaserWeapon>(LaserClass, GetActorTransform(), SpawnParams);
		if (Laser)
		{
			Laser->AttachToComponent(Camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Laser->SetActorRelativeLocation(LaserOffset);
		}
	}
}

void ASSPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Laser)
	{
		Laser->Destroy();
		Laser = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ASSPlayerCharacter::BuildDefaultInput()
{
	const auto MakeAction = [this](const TCHAR* Name, EInputActionValueType ValueType)
	{
		UInputAction* Action = NewObject<UInputAction>(this, Name);
		Action->ValueType = ValueType;
		return Action;
	};

	MoveAction = MakeAction(TEXT("IA_Move"), EInputActionValueType::Axis2D);
	LookAction = MakeAction(TEXT("IA_Look"), EInputActionValueType::Axis2D);
	JumpAction = MakeAction(TEXT("IA_Jump"), EInputActionValueType::Boolean);
	InteractAction = MakeAction(TEXT("IA_Interact"), EInputActionValueType::Boolean);
	FireAction = MakeAction(TEXT("IA_Fire"), EInputActionValueType::Boolean);
	GogglesAction = MakeAction(TEXT("IA_Goggles"), EInputActionValueType::Boolean);
	FrequencyAction = MakeAction(TEXT("IA_Frequency"), EInputActionValueType::Boolean);

	DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Default"));

	// Move is (X = right, Y = forward). A key press arrives on X, so forward and
	// back swizzle it onto Y, and back and left negate it.
	const auto MapMoveKey = [this](FKey Key, bool bSwizzle, bool bNegate)
	{
		FEnhancedActionKeyMapping& Mapping = DefaultMappingContext->MapKey(MoveAction, Key);
		if (bSwizzle)
		{
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			Mapping.Modifiers.Add(Swizzle);
		}
		if (bNegate)
		{
			Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(DefaultMappingContext));
		}
	};

	MapMoveKey(EKeys::W, true, false);
	MapMoveKey(EKeys::S, true, true);
	MapMoveKey(EKeys::D, false, false);
	MapMoveKey(EKeys::A, false, true);

	// Mouse Y is positive upwards; pitch input is positive downwards.
	FEnhancedActionKeyMapping& LookMapping = DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);
	UInputModifierNegate* InvertPitch = NewObject<UInputModifierNegate>(DefaultMappingContext);
	InvertPitch->bX = false;
	InvertPitch->bY = true;
	InvertPitch->bZ = false;
	LookMapping.Modifiers.Add(InvertPitch);

	DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);
	DefaultMappingContext->MapKey(InteractAction, EKeys::E);
	DefaultMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
	DefaultMappingContext->MapKey(GogglesAction, EKeys::RightMouseButton);
	DefaultMappingContext->MapKey(FrequencyAction, EKeys::Q);
}

void ASSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input)
	{
		return;
	}

	BuildDefaultInput();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASSPlayerCharacter::Move);
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASSPlayerCharacter::Look);
	Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	Input->BindAction(InteractAction, ETriggerEvent::Started, this, &ASSPlayerCharacter::Interact);
	Input->BindAction(FireAction, ETriggerEvent::Started, this, &ASSPlayerCharacter::StartFire);
	Input->BindAction(FireAction, ETriggerEvent::Completed, this, &ASSPlayerCharacter::StopFire);
	Input->BindAction(FireAction, ETriggerEvent::Canceled, this, &ASSPlayerCharacter::StopFire);
	Input->BindAction(GogglesAction, ETriggerEvent::Started, this, &ASSPlayerCharacter::ToggleGoggles);
	Input->BindAction(FrequencyAction, ETriggerEvent::Started, this, &ASSPlayerCharacter::CycleFrequency);
}

void ASSPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), Axis.Y);
	AddMovementInput(GetActorRightVector(), Axis.X);
}

void ASSPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ASSPlayerCharacter::Interact()
{
	Interaction->TryInteract();
}

void ASSPlayerCharacter::StartFire()
{
	if (Laser)
	{
		Laser->StartFire();
	}
}

void ASSPlayerCharacter::StopFire()
{
	if (Laser)
	{
		Laser->StopFire();
	}
}

void ASSPlayerCharacter::ToggleGoggles()
{
	Goggles->ToggleGoggles();
}

void ASSPlayerCharacter::CycleFrequency()
{
	if (Laser)
	{
		Laser->CycleFrequency();
	}
}
