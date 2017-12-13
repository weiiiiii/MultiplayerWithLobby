// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "ParticleTrail.h"
// include STL
// includes Engine files
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UParticleTrail::UParticleTrail()
	: ParticleTrailsToSpawn()
	, particlesSpawned()
{

}

bool UParticleTrail::DeactivateTrails( bool destroyOwner )
{
	bool result = false;
	for ( auto particleSystem : particlesSpawned )
	{
		if ( destroyOwner )
			particleSystem->OnSystemFinished.AddDynamic( this, &UParticleTrail::removeParticleSystemAndDestroyOwner );
		else
			particleSystem->OnSystemFinished.AddDynamic( this, &UParticleTrail::removeParticleSystem );
		particleSystem->DeactivateSystem();
		result = true;
	}
	return result;
}

void UParticleTrail::BeginPlay()
{
	AActor* owner = GetOwner();
	// ensure owner exist
	check( nullptr != owner );
	// get and ensure world exist
	UWorld* world = owner->GetWorld();
	check( nullptr != world );

	USceneComponent* rootComponent = owner->GetRootComponent();
	FVector location = GetRelativeTransform().GetLocation();
	FRotator rotator = GetRelativeTransform().Rotator();
	for ( auto spawn : ParticleTrailsToSpawn )
	{
		if ( spawn.ParticleSystem )
		{
			UParticleSystemComponent* component = UGameplayStatics::SpawnEmitterAttached( spawn.ParticleSystem, rootComponent, "", location, rotator );
			component->SetRelativeScale3D( spawn.Scale );
			particlesSpawned.Push( component );
		}
	}
}

void UParticleTrail::removeParticleSystem( UParticleSystemComponent* psc )
{
	for ( auto particleSystem : particlesSpawned )
	{
		if ( particleSystem == psc )
		{
			particlesSpawned.Remove( psc );
			break;
		}
	}
}

void UParticleTrail::removeParticleSystemAndDestroyOwner( UParticleSystemComponent* psc )
{
	removeParticleSystem( psc );

	if ( 0 >= particlesSpawned.Num() )
	{
		AActor* owner = GetOwner();
		if ( nullptr != owner )
		{
			owner->Destroy();
		}
	}	
}
