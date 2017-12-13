// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <vector>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "WheelFXDataAsset.h"
// fixed final include
#include "VehicleEffectComponent.generated.h"

class UParticleSystemComponent;
class UWheeledVehicleMovementComponent;

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UVehicleEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Sound to play when game starts" ) )
		USoundCue* EngineSound;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Time to wait before playing engine sound" ) )
		float EngineSoundStartDelay;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Minimum angle between forward vector and velocity for vehicle to be considered skidding" ) )
		float SkidAngleThreshold;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Minimum speed for vehicle to skidding" ) )
		float SkidThresholdVelocity;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Minimum longitudinal slip for vehicle to skidding" ) )
		float LongSlipSkidThreshold;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Minimum lateral slip for vehicle to skidding" ) )
		float LateralSlipSkidThreshold;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Data asset to use when wheel comes in contact with various surfaces" ) )
		UWheelFXDataAsset* WheelFX;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Data asset to use when wheel comes in contact with various surfaces" ) )
		UWheeledVehicleMovementComponent* MovementComponent;
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Scene components used as placeholders to specify position where particle system components should be" ) )
		TArray<USceneComponent*> WheelParticlePlaceholders;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Minimum speed to spawn particles. WILL BE REMOVED!" ) )
		float minimumSpeedToSpawnParticles;

protected:
	bool isOnGround;
	std::vector<UParticleSystemComponent*> wheelParticles;



public:
	// Sets default values for this component's properties
	UVehicleEffectComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION( BlueprintCallable
		, meta = ( Category = "GameCloudSDKUE4|Vehicle", Tooltip = "Data asset to use when wheel comes in contact with various surfaces" ) )
		void SetupWheelEffects();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float getVehicleSpeed() const;

	void showMovementEffects( int wheelIndex, FMovementFXStruct movementFXStruct );
	void spawnSkidEffects( int wheelIndex, FSkidFXStruct skidFXStruct );
	void spawnWheelEffectsComponent( int wheelIndex );
	void updateWheelEffectsComponent( float deltaTime );
	
	void printVehicleStats();
};
