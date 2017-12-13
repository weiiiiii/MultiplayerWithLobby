// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "AutoDestroyEvent.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Weapon/ProjectileComponents/ParticleTrail.h"

UAutoDestroyEvent::UAutoDestroyEvent()
	: LifeSpan( 0.0f )
	, SpawnOnAutoDestroy()

	, lifespanHandle()
{
	//PrimaryComponentTick.bCanEverTick = true;
}

//void UAutoDestroyEvent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
//{
//	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
//}





void UAutoDestroyEvent::BeginPlay ()
{
	Super::BeginPlay ();
	setLifespanTimer();		
}

void UAutoDestroyEvent::setLifespanTimer()
{
	if ( 0.0f >= LifeSpan )
		return;

	AActor* owner = GetOwner();
	check( nullptr != owner );
	UWorld* world = owner->GetWorld();
	check( nullptr != world );
	world->GetTimerManager().SetTimer( lifespanHandle, this, &UAutoDestroyEvent::lifeOver, LifeSpan, false, LifeSpan );
}

void UAutoDestroyEvent::lifeOver()
{
	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	for ( auto spawn : SpawnOnAutoDestroy )
		spawn.SpawnObjects( owner );

	destroyActor();
}

void UAutoDestroyEvent::destroyActor()
{
	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	TArray<UActorComponent*> components;
	components = owner->GetComponentsByClass( UParticleTrail::StaticClass() );

	// owner does not have any particle trails to deactivate, proceed to destroy
	if ( 0 == components.Num() )
	{
		owner->Destroy();
		return;
	}

	bool areTrailsDeactivated = false;
	for ( UActorComponent* component : components )
	{
		UParticleTrail* trail = Cast<UParticleTrail>( component );
		if ( nullptr == component )
			continue;

		if ( trail->DeactivateTrails() )
			areTrailsDeactivated = true;
	}

	if ( !areTrailsDeactivated )
	{
		owner->Destroy();
		return;
	}

	// hide 1st static mesh
	// hiding owner will hide particle trail irregardless of whether the trail has been detached or not
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>( owner->GetComponentByClass( UStaticMeshComponent::StaticClass() ) );
	if ( mesh )
		mesh->SetHiddenInGame( true );
	// disable collision
	UShapeComponent* collision = Cast<UShapeComponent>( owner->GetComponentByClass( UShapeComponent::StaticClass() ) );
	if ( collision )
	{
		collision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
		collision->SetHiddenInGame( true );
	}
}
