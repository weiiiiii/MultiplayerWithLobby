// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "SpawnPoint.h"
// include STL
// includes Engine files
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
//#include "Kismet/KismetSystemLibrary.h"				//Debug
// include plugin files
// includes third party files
// includes project files
//#include "GameCloudSDKUE4/Common/CommonFunctions.h"	//Debug
#include "GameCloudSDKUE4/Spawn/SpawnSystem.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
	: VolumeToSpawnIn_( nullptr )
	, ActorsToSpawn_()
	, MaxActiveCount_( 0 )
	, TotalSpawnCount_( 0 )
	, BaseDelayBetweenSpawn_( 0.0f )
	, AdditionalDelayRange_( 0.0f )
	, IsSpawningForever_( false )
	, DeactivateAfterTotalSpawnCountReached_( false )
	, CollisionChannel()
	, ParticleToSpawn_()
	, SoundToSpawn_()

	, IsActive_( false )
	, SpawnedActors_()
	, CurrentSpawnedCount_( 0 )
	, Cooldown_( 0.0f )
	, ParticleDelayLeft_( 0.0f )
	, SpawnSystem_( nullptr )

	, FindPointByShape( nullptr )
	, SpawnPointState_( SpawnPointState::INACTIVE )
	, ActorSpawned_( nullptr )
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASpawnPoint::Activate( bool flag )
{
	IsActive_ = flag;

	if ( flag )
	{ 
		// update SpawnPointState_
		FString reason;
		IsReadyToSpawn( SpawnPointState_, reason );
	}
}

bool ASpawnPoint::IsOwned()
{
	return nullptr != SpawnSystem_;
}

bool ASpawnPoint::IsReadyToSpawn( SpawnPointState& Result, FString& Reason )
{
	bool isReady = false;
	Reason = UKismetSystemLibrary::GetDisplayName( this );

	if ( IsActive_ )
	{
		if ( SpawnPointState::SPAWNING_PARTICLES == SpawnPointState_ )
		{ Reason += " is spawning particles"; }
		else
		{
			if ( 0.0f >= Cooldown_ )
			{
				if ( SpawnedActors_.Num() < MaxActiveCount_ )
				{
					if ( !IsSpawningForever_ && TotalSpawnCount_ <= CurrentSpawnedCount_ )
					{
						Reason += " has spawned maximum allowed actors";
						Result = SpawnPointState::TOTAL_SPAWN_COUNT_REACHED;
					}
					else
					{
						Result = SpawnPointState::READY;
						Reason += " is ready";
						isReady = true;
					}
				}
				else
				{
					Reason += " cannot spawn any more active actors";
					Result = SpawnPointState::MAX_ACTIVE_COUNT_REACHED;
				}
			}
			else
			{
				Reason += " is still cooling down";
				Result = SpawnPointState::COOLING_DOWN;
			}
		}		
	}
	else
	{
		Reason += " is not active";
		Result = SpawnPointState::INACTIVE;
	}

	return isReady;
}

void ASpawnPoint::ResetSpawnPoint()
{
	CurrentSpawnedCount_ = 0;
	Cooldown_ = 0.0f;
	ParticleDelayLeft_ = 0.0f;
	
	// temporary container to avoid messing up with loop while clearing spawned actors
	TMap<FString, AActor*> tempMap = SpawnedActors_;
	for ( TPair<FString, AActor*> pair : tempMap )
	{
		AActor* actor = pair.Value;
		if ( nullptr != actor )
		{
			actor->Destroy();
		}
	}
}

void ASpawnPoint::Spawn()
{
	if ( ensureAlwaysMsgf( IsActive_, *FString( "Activate spawn point before spawning!" ) ) )
	{
		auto world = GetWorld();
		check( world != nullptr );
		if ( ensureAlwaysMsgf( world != nullptr, *FString( "World is invalid!" ) ) )
		{
			if ( ensureAlwaysMsgf( 0.0f >= Cooldown_, *FString( "Check if SpawnPoint is ready with IsReadyToSpawn before spawning!" ) ) )
			{
				if ( ensureAlwaysMsgf( MaxActiveCount_ > SpawnedActors_.Num(), *FString( "Check if SpawnPoint is ready with IsReadyToSpawn before spawning!" ) ) )
				{
					if ( ensureAlwaysMsgf( IsSpawningForever_ || TotalSpawnCount_ > CurrentSpawnedCount_, *FString( "Check if SpawnPoint is ready with IsReadyToSpawn before spawning!" ) ) )
					{
						// find location for particles and actor to spawn
						FVector spawnLocationFound = ( this->*FindPointByShape )( );
						// try to spawn actor
						FActorSpawnParameters params = FActorSpawnParameters();
						params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
						TSubclassOf<AActor> classToSpawn = ActorsToSpawn_[UKismetMathLibrary::RandomInteger( ActorsToSpawn_.Num() )];
						check( nullptr != classToSpawn );
						ActorSpawned_ = world->SpawnActor<AActor>( classToSpawn, spawnLocationFound, GetActorRotation(), params );

						if ( nullptr != ActorSpawned_ )
						{
							if ( ensureAlwaysMsgf( nullptr != FindPointByShape, *FString( "Allow spawn point to run BeginPlay() before running Spawn()" ) ) )
							{
								// set location again
								spawnLocationFound = ActorSpawned_->GetActorLocation();
								// spawn particles
								if ( nullptr != ParticleToSpawn_.Particle_ )
								{
									UParticleSystemComponent* particle = UGameplayStatics::SpawnEmitterAtLocation( world, ParticleToSpawn_.Particle_, spawnLocationFound );
									if ( particle )
										particle->SetRelativeScale3D( ParticleToSpawn_.Scale_ );
									ParticleDelayLeft_ = ParticleToSpawn_.Delay_;
									SpawnPointState_ = SpawnPointState::SPAWNING_PARTICLES;
								}

								//play sound
								if ( nullptr != SoundToSpawn_.Sound_ )
								{ UGameplayStatics::PlaySoundAtLocation( world, SoundToSpawn_.Sound_, spawnLocationFound, SoundToSpawn_.VolumeMultiplier_ ); }

								// set up actor		
								ActorSpawned_->OnDestroyed.AddDynamic( this, &ASpawnPoint::ActorDestroyed );
								// set actor spawn counters
								SpawnedActors_.Add( UKismetSystemLibrary::GetDisplayName( ActorSpawned_ ), ActorSpawned_ );
								CurrentSpawnedCount_++;
								Cooldown_ = BaseDelayBetweenSpawn_;
								if ( 0.0 < AdditionalDelayRange_ )
									Cooldown_ += UKismetMathLibrary::RandomFloatInRange( 0.0f, AdditionalDelayRange_ );
								if ( SpawnPointState::SPAWNING_PARTICLES != SpawnPointState_ )
								{ SpawnPointState_ = SpawnPointState::COOLING_DOWN; }
								else
								{ ActorSpawned_->SetActorHiddenInGame( true ); }
							}
						}

						// enable Tick() only for individual SpawnPoint
						if ( nullptr == SpawnSystem_ )
						{ SetActorTickEnabled( true ); }
					}
				}
			}
		}
	}
	//checkf( IsActive_, *FString( "Activate spawn point before spawning!" ) );
	//auto world = GetWorld();
	//check( world != nullptr );

	//checkf( 0.0f >= Cooldown_, *FString( "Check if SpawnPoint is ready with IsReadyToSpawn before spawning!" ) );
	//checkf( MaxActiveCount_ > SpawnedActors_.Num(), *FString( "Check if SpawnPoint is ready with IsReadyToSpawn before spawning!" ) );
	//checkf( IsSpawningForever_ || TotalSpawnCount_ > CurrentSpawnedCount_, *FString( "Check if SpawnPoint is ready with IsReadyToSpawn before spawning!" ) );
}

void ASpawnPoint::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	if ( IsActive_ )
	{
		switch ( SpawnPointState_ )
		{
			case SpawnPointState::INACTIVE:
				checkf( false, *FString( "Actor tick should be disabled before / while deactivating spawn point!" ) );
				break;
			case SpawnPointState::READY:
				Spawn();
				break;
			case SpawnPointState::SPAWNING_PARTICLES:
				if ( 0.0f < ParticleDelayLeft_ )
				{ ParticleDelayLeft_ -= DeltaTime; }
				else
				{
					ActorSpawned_->SetActorHiddenInGame( false );
					SpawnPointState_ = SpawnPointState::COOLING_DOWN;
				}
				break;
			case SpawnPointState::COOLING_DOWN:
				if ( 0 < Cooldown_ )
				{ Cooldown_ -= DeltaTime; }
				else
				{
					// update SpawnPointState_
					FString reason;
					IsReadyToSpawn( SpawnPointState_, reason );
					if ( SpawnPointState::INACTIVE == SpawnPointState_ )
					{ SetActorTickEnabled( false ); }
				}
				break;
			case SpawnPointState::MAX_ACTIVE_COUNT_REACHED:
			case SpawnPointState::TOTAL_SPAWN_COUNT_REACHED:
				SetActorTickEnabled( false );
				break;
			default:
				break;
		}
	}
	else
	{
		bool stopTick	= true;
		if ( 0.0f < ParticleDelayLeft_ )
		{ 
			ParticleDelayLeft_ -= DeltaTime;
			stopTick	= false;
		}
		if ( 0 < Cooldown_ )
		{
			Cooldown_ -= DeltaTime;
			stopTick	= false;
		}
		if ( stopTick )
		{
			FString reason;
			IsReadyToSpawn( SpawnPointState_, reason );
			if ( SpawnPointState::INACTIVE == SpawnPointState_ )
			{ SetActorTickEnabled( false ); }
		}
	}
}

void ASpawnPoint::AddToSpawnSystem( ASpawnSystem* SpawnSystem )
{
	checkf( nullptr != SpawnSystem, *( "Spawn system [" + UKismetSystemLibrary::GetDisplayName( SpawnSystem ) + "] is null!" ) );
	checkf( nullptr == SpawnSystem_, *FString( "Spawn point is already owned by a spawn system! Check if spawn point is owned, with IsOwned() before adding to a spawn system!" ) );
	SpawnSystem_ = SpawnSystem;
}

SpawnPointState ASpawnPoint::GetSpawnPointState()
{ return SpawnPointState_; }

bool ASpawnPoint::IsCoolingDown()
{ return ( 0.0f < Cooldown_ ); }

void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	FString name = UKismetSystemLibrary::GetDisplayName( this );
	checkf( nullptr != VolumeToSpawnIn_, *FString( name + " must have a Volume To Spawn In defined in Construction Script!" ) );

	// set up function pointer, FindPointByShape
	USphereComponent* sphereComponent = Cast<USphereComponent>( VolumeToSpawnIn_ );
	if ( nullptr != sphereComponent )
	{ FindPointByShape = &ASpawnPoint::FindPointToSpawnInSphereVolume; }
	else
	{
		UBoxComponent* boxComponent = Cast<UBoxComponent>( VolumeToSpawnIn_ );
		if ( nullptr != boxComponent )
		{ FindPointByShape = &ASpawnPoint::FindPointToSpawnInBoxVolume; }
		else
		{ checkf( false, *FString( name + " contains a Volume To Spawn In is neither a Sphere Collision Component nor a Box Collision Component!" ) ); }
	}
}

FVector ASpawnPoint::FindPointToSpawnInSphereVolume()
{
	USphereComponent* sphereComponent = Cast<USphereComponent>( VolumeToSpawnIn_ );
	FVector location = sphereComponent->GetComponentLocation();
	float sphereRadius = sphereComponent->GetScaledSphereRadius();

	FVector randomVector = FVector( sphereRadius );
	randomVector.X *= UKismetMathLibrary::RandomFloatInRange( -1.0f, 1.0f );
	randomVector.Y *= UKismetMathLibrary::RandomFloatInRange( -1.0f, 1.0f );
	randomVector.Z *= UKismetMathLibrary::RandomFloatInRange( -1.0f, 1.0f );

	FVector toReturn = location + randomVector;
	return toReturn;
}

FVector ASpawnPoint::FindPointToSpawnInBoxVolume()
{
	UBoxComponent* boxComponent = Cast<UBoxComponent>( VolumeToSpawnIn_ );
	FVector scaledBoxExtent = boxComponent->GetScaledBoxExtent();
	FVector boxLocation = boxComponent->GetComponentLocation();
	float randomX = UKismetMathLibrary::RandomFloatInRange( scaledBoxExtent.X * -1.0f, scaledBoxExtent.X * 1.0f );
	float randomY = UKismetMathLibrary::RandomFloatInRange( scaledBoxExtent.Y * -1.0f, scaledBoxExtent.Y * 1.0f );
	FVector pointAtTop = boxLocation + FVector( randomX, randomY, scaledBoxExtent.Z );
	FVector pointAtBottom = pointAtTop - FVector( 0.0f, 0.0f, scaledBoxExtent.Z * 2.0f  );

	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, scaledBoxExtent.ToCompactString() );
	//DrawDebugSphere( GetWorld(), pointAtTop, 10, 20, FColor::Blue, true, 10.0f );

	// line cast down from pointAtTop
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams( FName( TEXT( "RV_Trace" ) ), true, this );
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;
	//Re-initialize hit info
	FHitResult RV_Hit( ForceInit );
	UWorld* world = GetWorld();
	check( nullptr != world );
	const ECollisionChannel & Channel = UCollisionProfile::Get()->ConvertToCollisionChannel( false, CollisionChannel );
	world->LineTraceSingleByChannel(
		RV_Hit,						//result
		pointAtTop,					//start
		pointAtBottom,				//end
		Channel,					//collision channel
		RV_TraceParams
	);
	
	FVector toReturn;
	if ( RV_Hit.IsValidBlockingHit() )
	{ toReturn = RV_Hit.ImpactPoint; }

	return toReturn;
}

void ASpawnPoint::ActorDestroyed( AActor* DestroyedActor )
{
	SpawnedActors_.Remove( UKismetSystemLibrary::GetDisplayName( DestroyedActor ) );

	// update SpawnPointState_
	FString reason;
	IsReadyToSpawn( SpawnPointState_, reason );

	// inform SpawnSystem
	if ( nullptr != SpawnSystem_ )
	{ SpawnSystem_->SpawnDestroyed(); }
	else
	{
		if ( IsActive_ )
		{ SetActorTickEnabled( true ); }
	}
}
