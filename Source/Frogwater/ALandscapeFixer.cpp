// Fill out your copyright notice in the Description page of Project Settings.


#include "ALandscapeFixer.h"

#include "Landscape.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AALandscapeFixer::AALandscapeFixer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AALandscapeFixer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FixLandscape();
}

// Called when the game starts or when spawned
void AALandscapeFixer::BeginPlay()
{
	Super::BeginPlay();

	FixLandscape();
}

// Called every frame
void AALandscapeFixer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AALandscapeFixer::FixLandscape()
{
	auto Landscape = Cast<ALandscape>(UGameplayStatics::GetActorOfClass(this, ALandscape::StaticClass()));
	if (!IsValid(Landscape))
	{
		UE_LOG(LogTemp, Warning, TEXT("LandscapeFixer: No Landscape found to fix."));
		return;
	}

	Landscape->SetLayerVisibility(1, false);
}
