// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DitheringComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROGWATER_API UDitheringComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDitheringComponent();
	virtual void OnRegister() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering")
	UMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering")
	bool bUseFloatCurve = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering", meta=(EditCondition="bUseFloatCurve", EditConditionHides))
	FRuntimeFloatCurve SpawnDitherCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering", meta=(EditCondition="bUseFloatCurve", EditConditionHides))
	FRuntimeFloatCurve DestroyDitherCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering", meta=(EditCondition="!bUseFloatCurve", EditConditionHides))
	float SpawnDitherTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering", meta=(EditCondition="!bUseFloatCurve", EditConditionHides))
	float DestroyDitherTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dithering", AdvancedDisplay)
	FName MaterialAttributeName = TEXT("Dithering Amount");

	uint8 LastDitherType;
	FTimerHandle DitherTimer;
	
protected:
	void AssignMesh();
	void InitialiseDither() const;

	void StartDitherSpawn();
	void StartDitherDestroy();
	
	void DitherSpawnUpdate() const;
	void DitherDestroyUpdate() const;
};
