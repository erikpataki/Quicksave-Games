// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularComponent.h"

// Sets default values for this component's properties
UModularComponent::UModularComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UModularComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetOwner()->OnActorBeginOverlap.AddUniqueDynamic(this, &UModularComponent::OnActorBeginOverlap);
	GetOwner()->OnActorEndOverlap.AddUniqueDynamic(this, &UModularComponent::OnActorEndOverlap);
}


// Called every frame
void UModularComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UModularComponent::OnActorEndOverlap_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{
}

void UModularComponent::OnActorBeginOverlap_Implementation(AActor* OverlappedActor, AActor* OtherActor)
{
}

