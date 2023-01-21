// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROGWATER_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	bool Interact(AController* InInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	void StopInteract(AController* InInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	void OnOffhand(AController* InInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	void OnHolstered(AController* InInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	void OnAim(AController* InInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	bool InteractRequiresAiming();
};
