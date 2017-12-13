// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Entity/FEntityData.h"
#include "GameCloudSDKUE4/Event/DebugOnEvent.h"
// fixed final include
#include "EntityComponent.generated.h"

// delegate functions
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnEntityKill, AActor*, KilledActor );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnEntityDestroy, AActor*, KilledActor );

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UEntityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "When health of this entity drops to zero or lesser, this delegate will broadcast" ) )
		FOnEntityKill OnEntityKill;
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "A failsafe in case Destroy() or DestroyActor is called before OnEntityKill gets to broadcast. Broadcasts on actor's OnDestroyed event" ) )
		FOnEntityDestroy OnEntityDestroy;
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
