// © 2021 Vixentail, Liam Hall and Savir Sohal

#pragma once

#include "CoreMinimal.h"
#include "HermesInteractableComponent.h"
#include "Components/CapsuleComponent.h"
#include "HermesInteractorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HERMES_API UHermesInteractorComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHermesInteractorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


// Might make this into its own system. For some reason, SetActive never fucking works for me. So this is my replacement.
public:
	UFUNCTION(BlueprintCallable, Category=Interact)
	bool IsEnabled() const { return bEnabled; }

	UFUNCTION(BlueprintCallable, Category=Interact)
	void Enable(bool bReset);

	UFUNCTION(BlueprintCallable, Category=Interact)
	void Disable();

	UFUNCTION(BlueprintCallable, Category=Interact)
	void SetEnabled(bool bNewEnabled, bool bReset = false);

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_IsEnabled, Category=Interact)
	bool bEnabled = true;

	UFUNCTION()
	void OnRep_IsEnabled();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHermesInteractorOnEnabledSignature,
	const UHermesInteractorComponent*, Sender,
	const bool, bIsEnabled);

	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesInteractorOnEnabledSignature OnEnabledChanged;
	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSet<UHermesInteractableComponent*> AvailableInteractableComps;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	UHermesInteractableComponent* FocusedInteractableComp = nullptr;

	FTimerHandle InteractTimer;
	
	
public:	
	void AddFocusableInteractableComponent(UHermesInteractableComponent* NewInteractableComponent);
	
	void RemoveFocusableInteractableComponent(UHermesInteractableComponent* NewInteractableComponent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	bool IsInteracting() const
	{
		return IsValid(FocusedInteractableComp) && GetWorld()->GetTimerManager().TimerExists(InteractTimer);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	UHermesInteractableComponent* GetFocusedComponent() const { return FocusedInteractableComp; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Interact)
	UHermesInteractableComponent* GetInteractingComponent() const;

	UFUNCTION(BlueprintCallable, Category=Interact)
	void StartInteraction();

	UFUNCTION(BlueprintCallable, Category=Interact)
	void CancelInteraction();

	bool CancelInteractionImplementation();

	UFUNCTION()
	void CompleteInteract();
	UFUNCTION(Server, Reliable)
	void ServerCompleteInteract(UHermesInteractableComponent* InteractableComponent);

protected:
	UHermesInteractableComponent* GetMostSuitableFocusableInteractComponent();
	
	void SetFocusedInteractableComponent(UHermesInteractableComponent* NewInteractableComponent);

	void HandleInteractableComponentOnEnter(UHermesInteractableComponent* NewInteractableComponent);

	void HandleInteractableComponentOnExit(UHermesInteractableComponent* NewInteractableComponent);

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHermesInteractorOnInstigatorSignature,
		const UHermesInteractorComponent*, Sender,
		const UHermesInteractableComponent*, InteractableComponent);

	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesInteractorOnInstigatorSignature OnFocus;

	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesInteractorOnInstigatorSignature OnUnfocus;
	
	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesInteractorOnInstigatorSignature OnInteractionStarted;

	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesInteractorOnInstigatorSignature OnInteractionCancelled;

	UPROPERTY(BlueprintAssignable, Category=Interact)
	FHermesInteractorOnInstigatorSignature OnInteractionComplete;
	
protected:
	UFUNCTION()
	void HandleOnFocusedInteractionStarted(const UHermesInteractableComponent* Sender, const AActor* Instigator);

	UFUNCTION()
	void HandleOnFocusedInteractionComplete(const UHermesInteractableComponent* Sender, const AActor* Instigator);

	UFUNCTION()
	void HandleOnFocusedInteractionCancelled(const UHermesInteractableComponent* Sender, const AActor* Instigator);
};
