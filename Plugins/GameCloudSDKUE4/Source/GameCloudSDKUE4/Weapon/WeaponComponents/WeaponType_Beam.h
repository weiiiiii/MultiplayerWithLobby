// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "WeaponType_Range.h"
// fixed final include
#include "WeaponType_Beam.generated.h"

class UTraceEvent;
UCLASS( ClassGroup = ( GameCloudSDKUE4 ) )
class GAMECLOUDSDKUE4_API UWeaponType_Beam : public UWeaponType_Range
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Distance that the beam(s) can reach, TO BE CHANGED, USE EffectiveRange_ FROM WeaponType_Range OR READ FROM DATA" ) )
		float BeamRange;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Array of beams to move when weapon is shot" ) )
		TArray<AActor*> Beams;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Starting points of each beams in Beams" ) )
		TArray<USceneComponent*> ProjectileSources;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Target points of each beams in Beams" ) )
		TArray<USceneComponent*> ProjectileTargets;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Collision component for each beams in Beams" ) )
		TArray<UPrimitiveComponent*> ProjectileColliders;
// 	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
// 		TArray<UTraceEvent*> TraceEvents;
	// test variable in attempt to get surface type on collision / overlap, using ProjectileColliders for now
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Test array, do not use" ) )
		TArray<AActor*> ColliderSpheres;

protected:

public:
	UWeaponType_Beam();
	virtual void ShootStart() override;
	virtual void ShootEnd() override;

protected:
	void BroadcastHit( FHitResult HitResult );
	virtual void Shoot() override;
	FHitResult Trace( FVector start, FVector end, ECollisionChannel CollisionChannel );
};
