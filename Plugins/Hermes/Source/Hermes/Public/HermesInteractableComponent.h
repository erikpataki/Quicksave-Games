// © 2021 Vixentail, Liam Hall and Savir Sohal

#pragma once

#include "CoreMinimal.h"
#include "HermesInteractMode.h"
#include "HermesInteractPriority.h"
#include "HermesNetMode.h"
#include "Components/BoxComponent.h"
#include "HermesInteractableComponent.generated.h"

#define INTERACTABLE_CHANNEL		ECollisionChannel::ECC_GameTraceChannel3

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HERMES_API UHermesInteractableComponent : public UBoxComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHermesInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY(EditAnywhere, Category=Interact)
	EHermesInteractMode Mode = EHermesInteractMode::Manual;

	UPROPERTY(EditAnywhere, Category=Interact)
	EHermesInteractPriority Priority = EHermesInteractPriority::Normal;

	UPROPERTY(EditAnywhere, Category=Interact)
	float InteractTime = 1.f;

	UPROPERTY(EditAnywhere, Category=Interact)
	FText FocusText = FText();

	UPROPERTY(EditAnywhere, Category=Interact)
	FText InteractingText = FText();
	
	UPROPERTY(EditAnywhere, Category=Interact)
	EHermesNetMode DestroyAfterInteraction = EHermesNetMode::None;

	// Not implemented.
	UPROPERTY(EditAnywhere, Category=Interact)
	bool bInstantKeyboard = false;

	// Not implemented.
	UPROPERTY(EditAnywhere, Category=Interact)
	USoundBase* InteractSound = nullptr;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	EHermesInteractMode GetInteractMode() const { return Mode; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	EHermesInteractPriority GetInteractPriority() const { return Priority; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	float GetInteractTime() const { return InteractTime; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	const FText& GetFocusText() const { return FocusText; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	const FText& GetInteractingText() const { return InteractingText; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	EHermesNetMode GetDestroyAfterInteraction() const { return DestroyAfterInteraction; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	bool IsEnabled() const;

public:
	/** Attempts to Interact with this Interactable Actor.
	 *  If Interacting from a Player, it is ideal to call 'StartInteract()' first and then after certain conditions have
	 *  been met (such as InteractTime reached), call 'Interact()'.
	 *  
	 *  Unless you can guarantee that this Component will be owned by a Client (likely not in most use cases), this
	 *  function will need to be called by the Server.
	 */
	UFUNCTION(BlueprintCallable, Category=Interact)
	void Interact(const AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category=Interact)
	bool StartInteract(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category=Interact)
	void CancelInteract(AActor* Instigator);

	/** Attempt to Focus this Interactable Actor.
	 *  Returns true as well as calling OnFocus event if it was able to be Focused.
	 */
	UFUNCTION(BlueprintCallable, Category=Interact)
	bool Focus(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category=Interact)
	void Unfocus(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category=Interact)
	void NotifyOverlap(AActor* Instigator);

protected:
	UFUNCTION(Server, Reliable, Category=Interact)
	void Server_Interact(const AActor* Instigator);

	UFUNCTION(NetMulticast, Reliable, Category=Interact)
	void SendInteractionComplete(const AActor* Instigator);


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHermesOnInstigatorSignature,
		const UHermesInteractableComponent*, Sender,
		const AActor*, Instigator);

	/** Use to enable client-side or server-side logic, such as auto-interact.
	 */
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesOnInstigatorSignature OnOverlap;

	/** Use to enable client-side logic, such as highlighting to the object.
	 *  It is not replicated to server.
	 */
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesOnInstigatorSignature OnFocus;

	/** Use to disable client-side logic, such as highlighting to the object.
	 *  It is not replicated to server.
	 */
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesOnInstigatorSignature OnUnfocus;

	// Multicast
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesOnInstigatorSignature OnInteractionComplete;

	/** Use to enable client-side logic, such as the remaining interaction time to the object.
	 *  It is not replicated to server.
	 *  Maybe add option to enable multicast replication for certain objects?
	 */
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesOnInstigatorSignature OnInteractionStarted;

	/** Use to enable client-side logic when interaction stops.
	 *  It is not replicated to server.
	 */
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesOnInstigatorSignature OnInteractionCancelled;
};