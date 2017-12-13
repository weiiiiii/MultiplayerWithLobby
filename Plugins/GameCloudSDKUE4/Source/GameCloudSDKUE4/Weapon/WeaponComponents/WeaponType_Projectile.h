// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "WeaponType_Range.h"
// fixed final include
#include "WeaponType_Projectile.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ) )
class GAMECLOUDSDKUE4_API UWeaponType_Projectile : public UWeaponType_Range
{
	GENERATED_BODY()

public:
	virtual void Shoot() override;
};
