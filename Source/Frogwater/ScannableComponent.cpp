// Fill out your copyright notice in the Description page of Project Settings.


#include "ScannableComponent.h"

#include "QuestSubsystem.h"

// Sets default values for this component's properties
UScannableComponent::UScannableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	// Disable collision by default. Will be enabled at a later stage.
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Only respond to scannable and visibility channels.
	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionResponseToChannel(SCANNABLE_CHANNEL, ECR_Block);
	UPrimitiveComponent::SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}


// Called when the game starts
void UScannableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (bEnableAtStart)
		Enable();
}


// Called every frame
void UScannableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (State == EScannableState::Focused)
	{
		// Not been focused for a while.
		const FTimespan TimeDiff = FDateTime::UtcNow() - LastFocusTime;
		if (TimeDiff.GetTotalSeconds() >= TimeTillUnfocused)
		{
			State = EScannableState::Unfocused;
			OnCameraLostFocus.Broadcast(this, nullptr);
		}
	}
	
	TimeFocused = State == EScannableState::Focused ? TimeFocused + DeltaTime : 0;
	TimeUnfocused = State == EScannableState::Unfocused ? TimeUnfocused + DeltaTime : 0;
}

void UScannableComponent::Enable()
{
	// Enable the collision.
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

bool UScannableComponent::ExecPictureTaken(ACamera* Camera)
{
	// Picture already taken. This shouldn't happen if collision profiles are working correctly.
	if (bPictureTaken)
		return false;

	State = EScannableState::Selected;
	bPictureTaken = true;

	if (IsValid(Camera) && bCompleteQuestUponPictureTaken && bSavePictureToQuest)
	{
		Camera->SaveLastCaptureAsFile(QuestName.ToString());
		GetWorld()->GetSubsystem<UQuestSubsystem>()->CompleteQuestById(QuestName);
	}

	OnPictureTaken.Broadcast(this, Camera);
	
	DestroyComponent();

	return true;
}

void UScannableComponent::ExecIsFocus(ACamera* Camera)
{
	State = EScannableState::Focused;
	LastFocusTime = FDateTime::UtcNow();
	OnCameraFocus.Broadcast(this, Camera);
}

void UScannableComponent::ExecIsOuterFocus(ACamera* Camera)
{
	State = EScannableState::Focusing;
	OnCameraOuterFocus.Broadcast(this, Camera);
}