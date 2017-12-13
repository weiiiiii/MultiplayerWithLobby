// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Weapon/BaseProjectile_CPP.h"
// fixed final include
#include "WeaponType_Range.generated.h"

class USceneComponent;

UCLASS( ClassGroup = ( GameCloudSDKUE4 ) )
class GAMECLOUDSDKUE4_API UWeaponType_Range : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Spawn point(s) of projectiles" ) )
		TArray<USceneComponent*> ProjectileSpawnPoints;
	// Type of projectile to spawn
	TSubclassOf<AActor> ProjectileType;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		TArray<USceneComponent*> EffectSpawnPoints;
	/**	Range where projectile can deal damage. */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "GameCloudSDKUE4|Weapon|WeaponComponents" )
		float EffectiveRange;
	/**	Channel the beam has to trace in. */
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, Category = "GameCloudSDKUE4|Weapon|WeaponComponents" )
		TEnumAsByte<ECollisionChannel> CollisionChannel;
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Targets in this range may be hit by projectile, NOT COMPLETED!" ) )
		float ArcOfFire;
	FRotator WeaponSpread;

protected:
	bool HasShot;
	TArray<FRotator> ProjectileSpawnPointsRotation;

public:
	UWeaponType_Range();
	bool GetHasShot();
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Logic|Weapon|Weapon Components"
		, ToolTip = "Keeps track of all projectile spawn points and their original rotation" ) )
		void SetupProjectileSpawnPoints( TArray<USceneComponent*> SceneComponents );
	virtual void ShootStart();
	virtual void ShootUpdate();
	virtual void ShootEnd();

protected:
	FVector GetPawnVelocity();
	void RotateProjectileSpawnPoints();
	virtual void Shoot();
};
