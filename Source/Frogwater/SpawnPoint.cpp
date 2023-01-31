// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

// © 2021 Vixentail, Liam Hall and Savir Sohal


#include "SpawnPoint.h"

#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/CapsuleComponent.h"


ASpawnPoint::ASpawnPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(40.0f, 92.0f);
	// Stops projectiles from hitting the spawn point.
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetShouldUpdatePhysicsVolume(false);

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> PlayerStartTextureObject;
			FName ID_PlayerStart;
			FText NAME_PlayerStart;
			FName ID_Navigation;
			FText NAME_Navigation;
			FConstructorStatics()
				: PlayerStartTextureObject(TEXT("/Engine/EditorResources/S_Player"))
				, ID_PlayerStart(TEXT("PlayerStart"))
				, NAME_PlayerStart(NSLOCTEXT("SpriteCategory", "PlayerStart", "Player Start"))
				, ID_Navigation(TEXT("Navigation"))
				, NAME_Navigation(NSLOCTEXT("SpriteCategory", "Navigation", "Navigation"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		if (GetGoodSprite())
		{
			GetGoodSprite()->Sprite = ConstructorStatics.PlayerStartTextureObject.Get();
			GetGoodSprite()->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			GetGoodSprite()->SpriteInfo.Category = ConstructorStatics.ID_PlayerStart;
			GetGoodSprite()->SpriteInfo.DisplayName = ConstructorStatics.NAME_PlayerStart;
		}
		if (GetBadSprite())
		{
			GetBadSprite()->SetVisibility(false);
		}

		if (ArrowComponent)
		{
			ArrowComponent->ArrowColor = FColor(150, 200, 255);
			ArrowComponent->ArrowSize = 1.0f;
			ArrowComponent->bTreatAsASprite = true;
			ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Navigation;
			ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Navigation;
			ArrowComponent->SetupAttachment(GetCapsuleComponent());
			ArrowComponent->bIsScreenSizeScaled = true;
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void ASpawnPoint::OnConstruction(const FTransform& Transform)
{
	if (bExactSpawn)
	{
		// Enables overlap collision for pawn to trick spawn point into thinking the spawn point is invalid.
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
	}
	
	Super::OnConstruction(Transform);
}

void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ASpawnPoint::CanSpawn_Implementation(const AActor* Actor) const
{
	const bool bPassesCollisionCheck = !bExactSpawn || bExactSpawn && GetCapsuleComponent()->GetOverlapInfos().Num() == 0;
	
	return bPassesCollisionCheck;
}

bool ASpawnPoint::Spawn_Implementation(AActor* Actor)
{
	if (!IsValid(Actor) || !CanSpawn(Actor))
		return false;

	return Actor->TeleportTo(GetActorLocation(), GetActorRotation());
}

#if WITH_EDITORONLY_DATA

/** Returns ArrowComponent subobject **/
UArrowComponent* ASpawnPoint::GetArrowComponent() const { return ArrowComponent; }

#endif
