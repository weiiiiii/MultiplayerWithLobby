// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <map>
#include <functional>
// includes Engine files
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/EnumAsByte.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "SpawnPoint.generated.h"

class ASpawnSystem;
class UParticleSystem;
class UShapeComponent;
class USoundBase;

UENUM( BlueprintType )
enum class SpawnPointState : uint8
{
	INACTIVE,
	READY,
	SPAWNING_PARTICLES,
	COOLING_DOWN,
	MAX_ACTIVE_COUNT_REACHED,
	TOTAL_SPAWN_COUNT_REACHED
};

USTRUCT( BlueprintType )
struct FParticleSpawnPoint
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Particle system to spawn before actor is spawned" ) )
		UParticleSystem* Particle_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Scale of particle system" ) )
		FVector Scale_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( ToolTip = "Delay between spawning particle and actor" ) )
		float Delay_;

	FParticleSpawnPoint()
		: Particle_( nullptr )
		, Scale_( FVector( 1.0f ) )
		, Delay_( 0.0f )
	{

	}
};

USTRUCT( BlueprintType )
struct FSoundSpawnPoint
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "SoundBases to spawn when spawning particle" ) )
		USoundBase* Sound_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Volume multiplier to apply to sound" ) )
		float VolumeMultiplier_;

	FSoundSpawnPoint()
		: Sound_( NULL )
		, VolumeMultiplier_( 1.0f )
	{

	}
};

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API ASpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY( BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn" ) )
		UShapeComponent* VolumeToSpawnIn_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "List of actors to spawn. Chance for each actor to spawn depends on the number of repeated actors in list" ) )
		TArray<TSubclassOf<AActor>> ActorsToSpawn_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Maximum number of actors allowed in level at any time" ) )
		int MaxActiveCount_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Maximum number of actors allowed to be spawned" ) )
		int TotalSpawnCount_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Time to wait between each spawn" ) )
		float BaseDelayBetweenSpawn_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Random time to wait, on top of Base Delay Between Spawn" ) )
		float AdditionalDelayRange_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "If true, Max Spawn Count will be ignored" ) )
		bool IsSpawningForever_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "If true, Spawn Point will deactivate after current spawn count equals total spawn count" ) )
		bool DeactivateAfterTotalSpawnCountReached_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Layer to check when using Box Collision as Volume To Spawn In" ) )
		TEnumAsByte<ECollisionChannel> CollisionChannel;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Particle structure used for this spawn point" ) )
		FParticleSpawnPoint ParticleToSpawn_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Delay between spawning particle and actor" ) )
		FSoundSpawnPoint SoundToSpawn_;

	UPROPERTY( BlueprintReadOnly, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "True if spawn point is activated" ) )
		bool IsActive_;
	UPROPERTY( BlueprintReadOnly, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Actors spawned (and are still active in scene)" ) )
		TMap<FString, AActor*> SpawnedActors_;
	UPROPERTY( BlueprintReadOnly, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Number of actors spawned so far" ) )
		int CurrentSpawnedCount_;
	UPROPERTY( BlueprintReadOnly, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Time left till being able to Spawn()" ) )
		float Cooldown_;
	UPROPERTY( BlueprintReadOnly, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Time left till particle system delay is over" ) )
		float ParticleDelayLeft_;
	UPROPERTY( BlueprintReadOnly, meta = ( category = "GameCloudSDKUE4|Spawn"
		, ToolTip = "Spawn system that owns this spawn point" ) )
		ASpawnSystem* SpawnSystem_;

protected:
	// function pointer to function to use based on the type of VolumeToSpawnIn_
	FVector( ASpawnPoint::*FindPointByShape )();
	SpawnPointState SpawnPointState_;
	AActor* ActorSpawned_;

public:
	ASpawnPoint();

	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Activate to allow spawning, deactivate to recall active spawns" ) )
		void Activate( bool flag );
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Returns true if spawn point is already owned by a spawn system" ) )
		bool IsOwned();
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn", ExpandEnumAsExecs = "Result",
		ToolTip = "Returns true if spawn point is active, cooldowned, current active count is under max active count and max spawn count has not been exceeded" ) )
		bool IsReadyToSpawn( SpawnPointState& Result, FString& Reason );
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Resets current spawned count back to 0" ) )
		void ResetSpawnPoint();
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Check if spawn point is ready to spawn via IsReadyToSpawn() before calling spawn" ) )
		void Spawn();
	virtual void Tick( float DeltaTime ) override;

	void AddToSpawnSystem( ASpawnSystem* SpawnSystem );
	SpawnPointState GetSpawnPointState();
	bool IsCoolingDown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FVector FindPointToSpawnInSphereVolume();
	FVector FindPointToSpawnInBoxVolume();

	// callback function to bound to actor's OnDestroy event
	UFUNCTION()
		void ActorDestroyed( AActor* DestroyedActor );
};
