// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "WeaponType_Range.h"
// fixed final include
#include "WeaponType_RayCast.generated.h"

/**
 * 
 */
UCLASS()
class GAMECLOUDSDKUE4_API UWeaponType_RayCast : public UWeaponType_Range
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Distance that the beam(s) can reach, TO BE CHANGED, USE EffectiveRange_ FROM WeaponType_Range OR READ FROM DATA" ) )
		float BeamRange;


protected:
	virtual void Shoot() override;
	FHitResult Trace( FVector start, FVector end );
};
