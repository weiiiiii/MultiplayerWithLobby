// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "SpawnOnShoot.h"
// include STL
// includes Engine files
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
// include plugin files
// includes third party files
// includes project files

USpawnOnShoot::USpawnOnShoot()
	: Particle( nullptr )
	, ParticleScale( FVector( 1.0f ) )
	, Sound( nullptr )
	, IsSpawnAttached( false )
{

}

void USpawnOnShoot::Spawn()
{
	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	UWorld* world = owner->GetWorld();
	if ( nullptr == world )
		return;

	FVector location = GetComponentLocation();
	if ( nullptr != Particle )
	{
		FRotator rotation = GetComponentRotation();
		UParticleSystemComponent* particle = nullptr;
		if ( IsSpawnAttached )
			particle = UGameplayStatics::SpawnEmitterAttached( Particle, this );
		else
			particle = UGameplayStatics::SpawnEmitterAtLocation( world, Particle, location, rotation );

		particle->SetRelativeScale3D( ParticleScale );
	}
	
	if ( nullptr != Sound )
	{
		if ( IsSpawnAttached )
			UGameplayStatics::SpawnSoundAttached( Sound, this );
		else
			UGameplayStatics::SpawnSoundAtLocation( world, Sound, location );
	}		
}

