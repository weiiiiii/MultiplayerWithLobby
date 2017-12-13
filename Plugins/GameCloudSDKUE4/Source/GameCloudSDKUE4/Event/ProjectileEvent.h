// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"
#include "Engine/DataTable.h"
// include plugin files
// includes third party files
// includes project files
#include "SpawnOnEvent.h"
#include "DebugOnEvent.h"
// final include
#include "ProjectileEvent.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UProjectileEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Bounce"
		, Tooltip = "Enable to allow spawning, creating decals, destroy owning actor and debug on event OnProjectileBounce. Projectile Bounce must first be enabled in projectile movement component!" ) )
		bool EnableOnBounce;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Bounce"
		, Tooltip = "SoundBases, actors and/or particles to spawn on event OnProjectileBounce" ) )
		TArray <FSpawnOnEvent> SpawnOnBounce;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Bounce"
		, Tooltip = "Table of surfaces and decal materials to spawn on event OnProjectileBounce" ) )
		UDataTable* DecalOnBounce;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Bounce"
		, Tooltip = "Destroy actor on event OnProjectileBounce. Sound will fade out and destroy if owner does not have any particle trail(s). If it has at least one, mesh of owner will be hidden with collision disabled and owner will destroy once all particles are destroyed." ) )
		bool DestroyActorOnBounce;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Bounce"
		, Tooltip = "Enables debug on event OnProjectileBounce" ) )
		FDebugOnEvent DebugOnBounce;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Stop"
		, Tooltip = "Enable to allow spawning, creating decals, destroy owning actor and debug on event OnProjectileStop" ) )
		bool EnableOnStop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Stop"
		, Tooltip = "SoundBases, actors and/or particles to spawn on event OnProjectileStop" ) )
		TArray <FSpawnOnEvent> SpawnOnStop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Stop"
		, Tooltip = "Table of surfaces and decal materials to spawn on event OnProjectileStop" ) )
		UDataTable* DecalOnStop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Stop"
		, Tooltip = "Destroy actor on event OnProjectileStop. Sound will fade out and destroy if owner does not have any particle trail(s). If it has at least one, mesh of owner will be hidden with collision disabled and owner will destroy once all particles are destroyed." ) )
		bool DestroyActorOnStop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Projectile Stop"
		, Tooltip = "Enables debug on event OnProjectileStop" ) )
		FDebugOnEvent DebugOnStop;

protected:
	UShapeComponent* shapeComponent;

public:
	UProjectileEvent();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnBounce( const FHitResult& Hit, const FVector& impactVelocity );
	UFUNCTION()
		void OnStop( const FHitResult& Hit );

	void spawnDecal( UDataTable* decalTable, FHitResult hitResult );
	void destroyActor();
};
