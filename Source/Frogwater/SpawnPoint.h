// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NavigationObjectBase.h"
#include "UObject/NoExportTypes.h"
#include "SpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API ASpawnPoint : public ANavigationObjectBase
{
	GENERATED_BODY()

public:
	ASpawnPoint(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	
	/** Used when searching for which playerstart to use. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Spawn)
	FString LocationTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Spawn)
	bool bActive = true;
	
	/** Enable if Enemies are only allowed to spawn exactly on the spawn point. No adjustments allowed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Spawn)
	bool bExactSpawn = false;
	

	/** Returns whether this spawn point can be used to spawn enemies. If an enemy class is given, it checks if that
	 * particular enemy can be spawned. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure, Category=Spawn, BlueprintNativeEvent)
	bool CanSpawn(const AActor* Actor = nullptr) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Spawn, BlueprintNativeEvent)
	bool Spawn(AActor* Actor);

	UFUNCTION( BlueprintPure, Category=Spawn)
	const FString& GetLocationTag() const { return LocationTag; }

	/** Arrow component to indicate forward direction of start */
	#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	class UArrowComponent* ArrowComponent;
public:
#endif

#if WITH_EDITORONLY_DATA
	/** Returns ArrowComponent subobject **/
	class UArrowComponent* GetArrowComponent() const;
#endif
};
