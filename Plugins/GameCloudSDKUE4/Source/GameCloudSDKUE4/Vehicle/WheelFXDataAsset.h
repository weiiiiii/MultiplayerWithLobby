// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "Containers/EnumAsByte.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "Materials/Material.h"
#include "Sound/SoundCue.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "WheelFXDataAsset.generated.h"

/**
 * 
 */


USTRUCT( BlueprintType )
struct FSkidFXStruct
{
	GENERATED_BODY()

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Decal material to be spawn when skidding" ) )
		UMaterial* Decal;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Time before decal starts to fade out" ) )
		float DecalFadeDelay;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Time taken for decal to fade out" ) )
		float DecalFadeOutDuration;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Scale of decal" ) )
		FVector DecalScale;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Sound cue to play when skidding" ) )
		USoundCue* Sound;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Time for sound to fade out" ) )
		float SoundFadeOut;

	FSkidFXStruct()
		: Decal( nullptr )
		, DecalFadeDelay( 0.0f )
		, DecalFadeOutDuration( 0.0f )
		, DecalScale( FVector::OneVector )
		, Sound( nullptr )
		, SoundFadeOut( 0.0f )
	{}
};

USTRUCT( BlueprintType )
struct FMovementFXStruct
{
	GENERATED_BODY()

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Particle to be spawn when moving / braking on surface" ) )
		UParticleSystem* Particle;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Scale of particle" ) )
		FVector Scale;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Spawn rate over speed graph" ) )
		UCurveFloat* SpawnRateCurve;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Maximum spawn rate of particle" ) )
		float MaxSpawnRate;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Speed required to reach maximum spawn rate of particle" ) )
		float MaxSpawnRateSpeed;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Sound cue to play when moving on surface" ) )
		USoundCue* Sound;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Boolean whether particle should be spawned when moving on surface" ) )
		bool SpawnParticlesOnMove;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Minimum speed to spawn brake particle when moving on surface" ) )
		float MinSpeedToSpawnBrakeParticles;

	FMovementFXStruct()
		: Particle( nullptr )
		, Scale( FVector::OneVector )
		, SpawnRateCurve( nullptr )
		, Sound( nullptr )
		, SpawnParticlesOnMove( true )
		, MinSpeedToSpawnBrakeParticles( 0.0f )
	{}
};

USTRUCT ( BlueprintType )
struct FWheelFXStruct
{
	GENERATED_BODY()

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Skid effect structure" ) )
		FSkidFXStruct SkidFX;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Particle to be spawn when moving on surface" ) )
		FMovementFXStruct MovementFX;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Tooltip = "Sound cue to play when landing on surface" ) )
		USoundCue* LandingSound;
};

UCLASS()
class GAMECLOUDSDKUE4_API UWheelFXDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		TArray<FWheelFXStruct> Effects;
	
};
