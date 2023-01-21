// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonScannableActor.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACommonScannableActor::ACommonScannableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	
	ScannableComponent = CreateDefaultSubobject<UScannableComponent>(TEXT("ScannableComp"));
	ScannableComponent->OnPictureTaken.AddUniqueDynamic(this, &ACommonScannableActor::OnPictureTaken);
	ScannableComponent->OnCameraFocus.AddUniqueDynamic(this, &ACommonScannableActor::OnCameraFocus);
	ScannableComponent->OnCameraOuterFocus.AddUniqueDynamic(this, &ACommonScannableActor::OnCameraOuterFocus);
	ScannableComponent->OnCameraLostFocus.AddUniqueDynamic(this, &ACommonScannableActor::OnCameraLostFocus);
	ScannableComponent->SetupAttachment(StaticMeshComponent);
}

// Called when the game starts or when spawned
void ACommonScannableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACommonScannableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACommonScannableActor::OnPictureTaken_Implementation(const UScannableComponent* Sender, const ACamera* Camera)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("Picture taken"));
}

void ACommonScannableActor::OnCameraFocus_Implementation(const UScannableComponent* Sender, const ACamera* Camera)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("Camera focused"));
}

void ACommonScannableActor::OnCameraFocusing_Implementation(const UScannableComponent* Sender, const ACamera* Camera)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("Camera focusing"));
}

void ACommonScannableActor::OnCameraOuterFocus_Implementation(const UScannableComponent* Sender, const ACamera* Camera)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("Camera outer focused"));
}

void ACommonScannableActor::OnCameraLostFocus_Implementation(const UScannableComponent* Sender,
	const ACamera* Camera)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("Camera lost focus"));
}
