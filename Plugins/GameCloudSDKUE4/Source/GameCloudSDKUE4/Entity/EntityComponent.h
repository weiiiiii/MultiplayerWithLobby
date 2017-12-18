// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Delegates/DelegateCombinations.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Entity/FEntityData.h"
#include "GameCloudSDKUE4/Event/DebugOnEvent.h"
// fixed final include
#include "EntityComponent.generated.h"

// delegate functions
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnEntityKill, AActor*, KilledActor, AController*, KilledBy );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnEntityDestroy, AActor*, KilledActor );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnEntityDamaged, float, DamageAmount );

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UEntityComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "When health of this entity drops to zero or lesser, this delegate will broadcast" ) )
		FOnEntityKill OnEntityKill;
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "A failsafe in case Destroy() or DestroyActor is called before OnEntityKill gets to broadcast. Broadcasts on actor's OnDestroyed event" ) )
		FOnEntityDestroy OnEntityDestroy;
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "When entity receives damage" ) )
		FOnEntityDamaged OnEntityDamaged;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Entity" ) )
		FEntityData EntityData;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Entity" , Tooltip = "Will call Destroy() on event OnEntityKill" ) )
		bool AutoDestroyOnEntityKill;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "If true, damage taken by actor owned by entity will be printed on screen" ) )
		FDebugOnEvent DebugDamageTaken;

protected:


public:
	UEntityComponent();
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EntityTakeDamage( float Damage, const class UDamageType* DamageType );
	UFUNCTION()
		void OnTakeAnyDamage( AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );
	UFUNCTION()
		void OnTakePointDamage( AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser );
	UFUNCTION()
		void OnDestroyed( AActor* DestroyedActor );
};
