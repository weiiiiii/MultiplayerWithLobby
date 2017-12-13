// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "SpawnSystem.h"
// include STL
// includes Engine files
#include "Kismet/KismetSystemLibrary.h"				//Debug
// include plugin files
// includes third party files
// includes project files
//#include "GameCloudSDKUE4/Common/CommonFunctions.h"	//Debug
#include "GameCloudSDKUE4/Spawn/SpawnPoint.h"

ASpawnSystem::ASpawnSystem()
	: MaxActiveCount_( 0 )
	, MaxSpawnCount_( 0 )
	, IsActive_( false )
	, IsSpawningForever_( false )

	, SpawnPoints_()

	, SpawnPointsToUpdate_()
	, CurrentIndex_( 0 )
	, CurrentActiveCount_( 0 )
	, CurrentSpawnedCount_( 0 )
	, AreSpawnPointsInactive_( true )
	, AreSpawnPointsMaxedOut_( false )
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawnSystem::AddSpawnPoint( ASpawnPoint* SpawnPoint )
{
	FString name = "Spawn Point [" + UKismetSystemLibrary::GetDisplayName( SpawnPoint ) + "]";
	checkf( nullptr != SpawnPoint, *( name + " is null!" ) );
	checkf( !SpawnPoint->IsOwned(), *( name + "is owned, check if spawn point is owned with IsOwned() before using AddToSpawnSystem" ) );
	
	if ( !SpawnPoints_.Contains( SpawnPoint ) )
	{ SpawnPoints_.Add( SpawnPoint ); }
	SpawnPoint->AddToSpawnSystem( this );
}

void ASpawnSystem::AddSpawnPoints( TArray<ASpawnPoint*> SpawnPoints )
{
	for ( ASpawnPoint* spawnPoint : SpawnPoints )
	{ AddSpawnPoint( spawnPoint ); }
}

void ASpawnSystem::ResetAll()
{
	for ( ASpawnPoint* spawnPoint : SpawnPoints_ )
	{ spawnPoint->ResetSpawnPoint(); }
}

//void ASpawnSystem::StopSpawning()
//{
//	SpawnPointsToUpdate_.clear();
//	SetActorTickEnabled( false );
//	for ( ASpawnPoint* spawnpoint : SpawnPoints_ )
//	{
//		spawnpoint->Activate();
//	}
//}

void ASpawnSystem::SpawnAll()
{
	// Do not run SpawnAll() in begin play of level. BeginPlay() of SpawnSystem will run AFTER
	// BeginPlay() of level, thus disabling tick of SpawnSystem.
	SetActorTickEnabled( true );
	SpawnPointsStartSpawning();
}

void ASpawnSystem::SpawnDestroyed()
{
	CurrentActiveCount_--;
	SetActorTickEnabled( true );
	SpawnPointsStartSpawning();
}

void ASpawnSystem::SpawnRecovered()
{
	CurrentActiveCount_--;
	CurrentSpawnedCount_--;
}

void ASpawnSystem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// update spawn points that require update
	if ( 0 < SpawnPointsToUpdate_.size() )
	{
		std::vector<int> toRemove;
		for ( std::map<int, ASpawnPoint*>::iterator iter = SpawnPointsToUpdate_.begin(); iter != SpawnPointsToUpdate_.end(); ++iter )
		{
			ASpawnPoint* spawnPoint = iter->second;
			spawnPoint->Tick( DeltaTime );
			SpawnPointState spawnPointState = spawnPoint->GetSpawnPointState();
			if ( SpawnPointState::COOLING_DOWN != spawnPointState && SpawnPointState::SPAWNING_PARTICLES != spawnPointState )
			{ toRemove.push_back( iter->first ); }
		}

		for ( int i = 0; i < toRemove.size(); i++ )
		{
			int indexToRemove = toRemove[i];
			SpawnPointsToUpdate_.erase( indexToRemove );
		}
	}

	SpawnPointsStartSpawning();

	// there exist no spawn point to update
	if ( 0 <= SpawnPointsToUpdate_.size() )
	{
		// system cannot spawn any more, disable tick
		if ( AreSpawnPointsMaxedOut_ || AreSpawnPointsInactive_ )
		{ SetActorTickEnabled( false ); }
	}
}

void ASpawnSystem::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled( false );

	for ( ASpawnPoint* spawnPoint : SpawnPoints_ )
	{
		if ( spawnPoint->IsOwned() )
		{
			ASpawnSystem* owningSpawnSystem = spawnPoint->SpawnSystem_;
			if ( this != owningSpawnSystem )
			{
				FString spawnSystemName = UKismetSystemLibrary::GetDisplayName( this );
				FString spawnPointName = UKismetSystemLibrary::GetDisplayName( spawnPoint );
				FString ownerName = UKismetSystemLibrary::GetDisplayName( spawnPoint->SpawnSystem_ );
				checkf( false, *FString( spawnPointName + " is already owned by " + spawnSystemName + ". Please ensure that " + spawnPointName + " is owned by only one SpawnSystem." ) );
			}
		}
		else
		{ AddSpawnPoint( spawnPoint ); }
	}
}

std::map<int, ASpawnPoint*> ASpawnSystem::GetReadySpawnPoints()
{
	// set to maxed out at start of function, if any of the spawn points are ready or cooling down, it will be set to false
	AreSpawnPointsMaxedOut_ = true;
	AreSpawnPointsInactive_ = true;

	// map of spawn points that are ready to spawn, using their indices as key in this map
	std::map<int, ASpawnPoint*> toReturn;
	// enum to gather result whether spawn point is ready to spawn
	SpawnPointState isReadyEnum;
	// FString for printing out result
	FString reason;

	for ( int i = 0; i < SpawnPoints_.Num(); i++ )
	{
		ASpawnPoint* spawnPoint = SpawnPoints_[i];
		if ( spawnPoint->IsReadyToSpawn( isReadyEnum, reason ) )
		{
			AreSpawnPointsMaxedOut_ = false;
			toReturn.insert( { i, spawnPoint } );
		}
		else if (	SpawnPointState::MAX_ACTIVE_COUNT_REACHED != isReadyEnum
				&& SpawnPointState::TOTAL_SPAWN_COUNT_REACHED != isReadyEnum )
		{ AreSpawnPointsMaxedOut_ = false; }
		
		if ( SpawnPointState::INACTIVE != isReadyEnum )
		{ AreSpawnPointsInactive_ = false; }
	}
	return toReturn;
}

bool ASpawnSystem::IsSystemReadyToSpawn()
{
	if ( MaxActiveCount_ > CurrentActiveCount_ )
	{
		if ( IsSpawningForever_ || MaxSpawnCount_ > CurrentSpawnedCount_ )
		{ return true; }
	}
	return false;
}

void ASpawnSystem::SpawnPointsStartSpawning()
{
	// system is ready to spawn, i.e. system has space for new spawns
	if ( IsSystemReadyToSpawn() )
	{
		std::map<int, ASpawnPoint*> readySpawnPoints = GetReadySpawnPoints();
		// there exist at least a spawn point that is ready, trigger spawn() 
		if ( 0 < readySpawnPoints.size() )
		{
			struct StartEnd
			{
				int start;
				int end;
			};

			std::vector<StartEnd> indices =
			{
				StartEnd { CurrentIndex_, (int)SpawnPoints_.Num() },
				StartEnd { 0, CurrentIndex_ }
			};

			int lastSuccessfulIndex = -1;
			for ( int i = 0; i < indices.size(); i++ )
			{
				int start = indices[i].start;
				int end = indices[i].end;
				for ( int j = start; j < end; j++ )
				{
					std::map<int, ASpawnPoint*>::iterator iter = readySpawnPoints.find( j );
					if ( readySpawnPoints.end() != iter )
					{
						ASpawnPoint* spawnPoint = iter->second;
						spawnPoint->Spawn();
						CurrentActiveCount_++;
						CurrentSpawnedCount_++;

						lastSuccessfulIndex = j;
						SpawnPointsToUpdate_.insert( { j, spawnPoint } );
					}
				}
			}

			// try to update current index
			if ( -1 != lastSuccessfulIndex )
			{ CurrentIndex_ = lastSuccessfulIndex + 1; }
		}
	}
}
