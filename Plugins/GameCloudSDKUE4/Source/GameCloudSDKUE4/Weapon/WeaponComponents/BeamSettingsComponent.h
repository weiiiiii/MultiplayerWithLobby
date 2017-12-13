// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "BeamSettingsComponent.generated.h"


UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UBeamSettingsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**	Distance reachable by beam. */
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, Category = "GameCloudSDKUE4|Weapon|WeaponComponents" )
		float Range;
	/**	Channel the beam has to trace in. */
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, Category = "GameCloudSDKUE4|Weapon|WeaponComponents" )
		TEnumAsByte<ECollisionChannel> CollisionChannel;

	UBeamSettingsComponent();
};
