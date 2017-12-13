// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "ProjectileEvent.h"
// include STL
// includes Engine files
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/FX/FFXDecalStruct.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Weapon/ProjectileComponents/ParticleTrail.h"

UProjectileEvent::UProjectileEvent()
	: EnableOnBounce( false )
	, SpawnOnBounce()
	, DecalOnBounce( nullptr )
	, DestroyActorOnBounce( false )

	, EnableOnStop( false )
	, SpawnOnStop()
	, DecalOnStop( nullptr )
	, DestroyActorOnStop( false )

	, shapeComponent( nullptr )
{
	//PrimaryComponentTick.bCanEverTick = true;
}

 void UProjectileEvent::BeginPlay()
 {
 	Super::BeginPlay();

	AActor* owner = GetOwner ();
	if ( owner )
	{
		shapeComponent = Cast<UShapeComponent>( owner->GetComponentByClass( UShapeComponent::StaticClass() ) );
		UProjectileMovementComponent* projectileMovement = Cast<UProjectileMovementComponent> ( owner->GetComponentByClass ( UProjectileMovementComponent::StaticClass () ) );
		if ( projectileMovement )
		{
			if ( EnableOnBounce )
				projectileMovement->OnProjectileBounce.AddDynamic( this, &UProjectileEvent::OnBounce );
			if ( EnableOnStop )
				projectileMovement->OnProjectileStop.AddDynamic( this, &UProjectileEvent::OnStop );
		}
	}
 }

// void UProjectileEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
// }

void UProjectileEvent::OnBounce( const FHitResult& Hit, const FVector& impactVelocity )
{
	DebugOnBounce.PrintOnHitEvent( "On Projectile Bounce", Hit );

	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	for ( auto spawn : SpawnOnBounce )
		spawn.SpawnObjects( owner );

	spawnDecal( DecalOnBounce, Hit );

	if ( DestroyActorOnBounce )
		destroyActor();
}

void UProjectileEvent::OnStop( const FHitResult& Hit )
{
	DebugOnStop.PrintOnHitEvent( "On Projectile Stop", Hit );

	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	for ( auto spawn : SpawnOnStop )
		spawn.SpawnObjects( owner );

	spawnDecal( DecalOnStop, Hit );

	if ( DestroyActorOnStop )
		destroyActor();
}

void UProjectileEvent::spawnDecal( UDataTable* decalTable, FHitResult hitResult )
{
	if ( nullptr == decalTable )
		return;

	TWeakObjectPtr<UPhysicalMaterial> physMat = hitResult.PhysMaterial;
	if ( nullptr == physMat )
	{
		UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "Spawning Decal : Physics material is nullptr" );
		return;
	}

	const FString & surfaceTypeName = UCommonFunctions::GetEnumValueToString( "EPhysicalSurface", physMat->SurfaceType );
	FFXDecalStruct* decalStruct = decalTable->FindRow<FFXDecalStruct>( FName( *surfaceTypeName ), "" );
	if ( nullptr == decalStruct )
	{
		UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "Spawning Decal : Decal data table row does not match FXDecalStruct!" );
		return;
	}

	AActor* owner = GetOwner();
	if ( nullptr == owner )
	{
		UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "Spawning Decal : Owner does not exist" );
		return;
	}

	FVector scale = FVector( 1.0f );
	if ( shapeComponent )
		scale = shapeComponent->GetComponentScale();
	decalStruct->SpawnImpactParticleFX( owner->GetWorld(), hitResult, scale );
	decalStruct->SpawnImpactDecals( owner->GetWorld(), hitResult, scale );
	decalStruct->PlayImpactSound( owner->GetWorld(), hitResult );
	decalStruct->SpawnDebrisParticleFX( owner->GetWorld(), hitResult, scale );
}

void UProjectileEvent::destroyActor()
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
