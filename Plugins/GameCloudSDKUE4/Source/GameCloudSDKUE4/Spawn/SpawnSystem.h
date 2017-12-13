// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <vector>
// includes Engine files
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "SpawnSystem.generated.h"

class ASpawnPoint;

// delegate functions
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnKill, AEntity*, Entity );

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API ASpawnSystem : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn" ) )
		int MaxActiveCount_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn" ) )
		int MaxSpawnCount_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn" ) )
		bool IsActive_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn", ToolTip = "If true, Max Spawn Count will be ignored" ) )
		bool IsSpawningForever_;
	
	// all spawn points owned by this spawn system
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Spawn", ToolTip = "If true, Max Spawn Count will be ignored" ) )
	TArray<ASpawnPoint*> SpawnPoints_;

protected:
	// all spawn points owned by this spawn system
	//std::vector<ASpawnPoint*> SpawnPoints_;
	// spawn points that have spawned and require update to cooldown
	std::map<int, ASpawnPoint*> SpawnPointsToUpdate_;
	// index of spawnPoint to spawn from
	int CurrentIndex_;
	int CurrentActiveCount_;
	int CurrentSpawnedCount_;
	bool AreSpawnPointsInactive_;
	bool AreSpawnPointsMaxedOut_;

public:
	ASpawnSystem();

	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Adds spawn point to spawn system" ) )
		void AddSpawnPoint( ASpawnPoint* SpawnPoint );
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Adds spawn point to spawn system" ) )
		void AddSpawnPoints( TArray<ASpawnPoint*> SpawnPoints );
	//UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
	//	ToolTip = "Activate / deactivate spawn points" ) )
	//	void StopSpawning();
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Resets counters and spawn point(s) counters" ) )
		void ResetAll();
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Spawn",
		ToolTip = "Triggers Spawn() for all spawn points until spawn limit has been reached" ) )
		void SpawnAll();
	void SpawnDestroyed();
	void SpawnRecovered();
	virtual void Tick( float DeltaTime ) override;


	// control max and current spawn count
	// inform someone that all enemies spawned died

protected:
	virtual void BeginPlay() override;
	// returns spawn points that are ready and sets AreSpawnPointsMaxedOut to true if all spawn points can't spawn any more
	std::map<int, ASpawnPoint*> GetReadySpawnPoints();
	bool IsSystemReadyToSpawn();
	void SpawnPointsStartSpawning();
};
