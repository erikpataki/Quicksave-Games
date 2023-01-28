// Copyright Epic Games, Inc. All Rights Reserved.

#include "FrogwaterCharacter.h"
#include "FrogwaterProjectile.h"
#include "TP_PickUpComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "BFFootstepsComponent.h"
#include "Kismet/KismetSystemLibrary.h"


//////////////////////////////////////////////////////////////////////////
// AFrogwaterCharacter

AFrogwaterCharacter::AFrogwaterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	FootstepsComponent = CreateDefaultSubobject<UBFFootstepsComponent>(TEXT("Footsteps Comp"));
}

void AFrogwaterCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void AFrogwaterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFrogwaterCharacter::OnPrimaryAction);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFrogwaterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFrogwaterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AFrogwaterCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AFrogwaterCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFrogwaterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFrogwaterCharacter::LookUpAtRate);
}


void AFrogwaterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AFrogwaterCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AFrogwaterCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFrogwaterCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFrogwaterCharacter::Turn(float Rate)
{
	AddControllerYawInput(Rate);

	CurrentTurn = Rate;
}

void AFrogwaterCharacter::LookUp(float Rate)
{
	AddControllerPitchInput(Rate);
	CurrentLookUp = Rate;
}

void AFrogwaterCharacter::TurnAtRate(float Rate)
{
	auto NewRate = Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(NewRate);
	CurrentTurnController = NewRate;
}

void AFrogwaterCharacter::LookUpAtRate(float Rate)
{
	auto NewRate = Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds();
	AddControllerPitchInput(NewRate);
	CurrentLookUpController = NewRate;
}