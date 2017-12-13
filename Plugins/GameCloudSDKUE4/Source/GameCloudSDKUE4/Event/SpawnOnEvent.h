// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
// fixed final include
#include "SpawnOnEvent.generated.h"

USTRUCT( BlueprintType )
struct FActorSpawnOnEvent
{
	GENERATED_USTRUCT_BODY ()

	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Actor to spawn on event" ) )
		TSubclassOf<AActor> Actor_;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Scale of actor to spawn on event" ) )
		FVector Scale_;

	FActorSpawnOnEvent()
		: Actor_( NULL )
		, Scale_( FVector( 1.0f ) )
	{

	}
};

USTRUCT( BlueprintType )
struct FParticleSpawnOnEvent
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Particle system to spawn on event" ) )
		UParticleSystem* Particle_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Scale of particle system to spawn on event" ) )
		FVector Scale_;

	FParticleSpawnOnEvent()
		: Particle_( NULL )
		, Scale_( FVector( 1.0f ) )
	{

	}
};

USTRUCT( BlueprintType )
struct FSoundSpawnOnEvent
{
	GENERATED_BODY()
		
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "SoundBases to spawn on event" ) )
		USoundBase* Sound_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Volume multiplier to apply to sound" ) )
		float VolumeMultiplier_;

	FSoundSpawnOnEvent()
		: Sound_( NULL )
		, VolumeMultiplier_( 1.0f )
	{

	}
};

USTRUCT( BlueprintType )
struct FSpawnOnEvent
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "SoundBases to spawn on event" ) )
		TArray<FSoundSpawnOnEvent> SoundStructs_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Actors to spawn on event" ) )
		TArray<FActorSpawnOnEvent> ActorStructs_;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Particle systems to spawn on event" ) )
		TArray<FParticleSpawnOnEvent> ParticleStructs_;

	void SpawnObjects( AActor* owner )
	{
		auto world = owner->GetWorld();
		if ( world )
		{
			FActorSpawnParameters params = FActorSpawnParameters();
			params.Instigator = owner->Instigator;

			const FRotator spawnRotation = owner->GetActorRotation();
			const FVector spawnLocation = owner->GetActorLocation();

			for ( auto toPlay : SoundStructs_ )
			{
				if ( toPlay.Sound_ )
					UGameplayStatics::PlaySoundAtLocation( world, toPlay.Sound_, spawnLocation, toPlay.VolumeMultiplier_ );
			}

			for ( auto toSpawn : ActorStructs_ )
			{
				if ( toSpawn.Actor_ )
				{
					AActor* spawned = world->SpawnActor<AActor> ( toSpawn.Actor_, spawnLocation, spawnRotation, params );
					if ( spawned )
					{
						spawned->SetActorScale3D( toSpawn.Scale_ );
						spawned->Instigator = owner->Instigator;
					}
				}
			}

			for ( auto toSpawn : ParticleStructs_ )
			{
				if ( toSpawn.Particle_ )
				{
					UParticleSystemComponent* particle = UGameplayStatics::SpawnEmitterAtLocation ( world, toSpawn.Particle_, spawnLocation );
					if ( particle )
						particle->SetRelativeScale3D ( toSpawn.Scale_ );
				}
			}
		}
	}
};


