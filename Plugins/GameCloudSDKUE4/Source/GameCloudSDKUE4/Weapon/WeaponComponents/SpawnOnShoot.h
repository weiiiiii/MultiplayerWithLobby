// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "SpawnOnShoot.generated.h"


UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API USpawnOnShoot : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Particle system to spawn on shoot" ) )
		UParticleSystem* Particle;
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Scale of particle system to spawn on shoot" ) )
		FVector ParticleScale;
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "Sound to spawn on shoot" ) )
		USoundCue* Sound;
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents"
		, Tooltip = "If true, particle system and sound cue spawned will be attached to this component" ) )
		bool IsSpawnAttached;

	USpawnOnShoot();
	UFUNCTION()
		void Spawn();	
};
