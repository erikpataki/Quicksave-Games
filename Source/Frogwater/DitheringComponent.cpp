// Fill out your copyright notice in the Description page of Project Settings.


#include "DitheringComponent.h"

// Sets default values for this component's properties
UDitheringComponent::UDitheringComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UDitheringComponent::OnRegister()
{
	Super::OnRegister();
	AssignMesh();

	// Don't initialise dither when editing world.
	if (GetWorld() && GetWorld()->IsGameWorld())
		InitialiseDither();
}


// Called when the game starts
void UDitheringComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	StartDitherSpawn();
}


// Called every frame
void UDitheringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (GetWorld()->GetTimerManager().IsTimerActive(DitherTimer))
	{
		if (LastDitherType == 1)
			DitherSpawnUpdate();
		else if (LastDitherType == 2)
			DitherDestroyUpdate();
	}
}

void UDitheringComponent::AssignMesh()
{
	// If no Mesh specified, find the first Mesh of the Actor.
	if (!MeshComponent)
		MeshComponent = Cast<UMeshComponent>(GetOwner()->GetComponentByClass(UMeshComponent::StaticClass()));	
}

void UDitheringComponent::InitialiseDither() const
{
	const float Value = bUseFloatCurve ? SpawnDitherCurve.GetRichCurveConst()->Eval(0) : 0;
	MeshComponent->SetScalarParameterValueOnMaterials(MaterialAttributeName, Value);
}

void UDitheringComponent::StartDitherSpawn()
{
	if (!IsValid(MeshComponent))
		return;

	LastDitherType = 1;
	
	if (bUseFloatCurve)
	{
		if (SpawnDitherCurve.GetRichCurveConst()->IsEmpty())
			return;

		const float MaxTime = SpawnDitherCurve.GetRichCurveConst()->Keys[SpawnDitherCurve.GetRichCurveConst()->Keys.Num()-1].Time;
		GetWorld()->GetTimerManager().SetTimer(DitherTimer, MaxTime, false);
	}
	else
		GetWorld()->GetTimerManager().SetTimer(DitherTimer, SpawnDitherTime, false);
}

void UDitheringComponent::StartDitherDestroy()
{
	if (!IsValid(MeshComponent))
		return;

	LastDitherType = 2;
	
	if (bUseFloatCurve)
	{
		if (DestroyDitherCurve.GetRichCurveConst()->IsEmpty())
			return;

		const float MaxTime = DestroyDitherCurve.GetRichCurveConst()->Keys[DestroyDitherCurve.GetRichCurveConst()->Keys.Num()-1].Time;
		GetWorld()->GetTimerManager().SetTimer(DitherTimer, MaxTime, false);
	}
	else
		GetWorld()->GetTimerManager().SetTimer(DitherTimer, DestroyDitherTime, false);
}

void UDitheringComponent::DitherSpawnUpdate() const
{
	if (!IsValid(MeshComponent))
		return;

	const float Value = bUseFloatCurve ?
        SpawnDitherCurve.GetRichCurveConst()->Eval(GetWorld()->GetTimerManager().GetTimerElapsed(DitherTimer)) : 
        (GetWorld()->GetTimerManager().GetTimerElapsed(DitherTimer) / SpawnDitherTime);
	
	MeshComponent->SetScalarParameterValueOnMaterials(MaterialAttributeName, Value);
}

void UDitheringComponent::DitherDestroyUpdate() const
{
	if (!IsValid(MeshComponent))
		return;

	const float Value = bUseFloatCurve ?
		1.f - DestroyDitherCurve.GetRichCurveConst()->Eval(GetWorld()->GetTimerManager().GetTimerElapsed(DitherTimer)) : 
		1.f - GetWorld()->GetTimerManager().GetTimerElapsed(DitherTimer) / DestroyDitherTime;

	MeshComponent->SetScalarParameterValueOnMaterials(MaterialAttributeName, Value);
}

