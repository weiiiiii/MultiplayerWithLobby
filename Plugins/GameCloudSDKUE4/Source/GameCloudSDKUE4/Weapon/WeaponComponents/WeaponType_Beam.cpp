// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WeaponType_Beam.h"
// include STL
// includes Engine files
#include "Components/ShapeComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Event/CollisionEvent.h"

UWeaponType_Beam::UWeaponType_Beam()
	: UWeaponType_Range()
{

}

void UWeaponType_Beam::ShootStart()
{
	if ( 0 != Beams.Num() )
	{
		UWeaponType_Range::ShootStart();

		for ( int i = 0; i < Beams.Num(); i++ )
		{
			AActor* beam = Beams[i];
			beam->SetActorHiddenInGame( false );
		}
		Shoot();
	}
}

void UWeaponType_Beam::ShootEnd()
{
	if ( 0 != Beams.Num() )	
	{
		UWeaponType_Range::ShootEnd();

		for ( int i = 0; i < Beams.Num(); i++ )
		{
			AActor* beam = Beams[i];
			beam->SetActorHiddenInGame( true );

			UPrimitiveComponent* collider = ProjectileColliders[i];
			if ( collider )
				collider->SetCollisionEnabled( ECollisionEnabled::Type::NoCollision );

			USceneComponent* target = ProjectileTargets[i];
			target->SetWorldLocation( beam->GetActorLocation() );
		}
	}
}

void UWeaponType_Beam::BroadcastHit( FHitResult HitResult )
{
	for ( int i = 0; i < Beams.Num(); i++ )
	{
		AActor* beam = Beams[i];
		UCollisionEvent* event	= Cast<UCollisionEvent>( beam->GetComponentByClass( UCollisionEvent::StaticClass() ) );
		if ( nullptr != event )
		{
			event->OnHit( nullptr, HitResult.GetActor(), HitResult.GetComponent(), FVector::ZeroVector, HitResult );
		}
	}
}

void UWeaponType_Beam::Shoot()
{
	UWeaponType_Range::Shoot();
	if ( 0 != Beams.Num() )
	{
		AActor* owner = GetOwner();
		check( owner != nullptr );

		auto world = owner->GetWorld();
		check( world != nullptr );

		for ( int i = 0; i < Beams.Num(); i++ )
		{
			AActor* beam = Beams[i];

			// get and set projectile's instigator
			APawn* instigator = owner->GetRootComponent()->GetAttachmentRootActor()->GetInstigator();
			beam->Instigator = instigator;

			UShapeComponent* projectileCollider = Cast<UShapeComponent>( beam->GetComponentByClass( UShapeComponent::StaticClass() ) );
			if ( projectileCollider )
				projectileCollider->IgnoreActorWhenMoving( instigator, true );

			FVector spawnStart = beam->GetActorLocation();
			FVector direction = beam->GetActorForwardVector();
			FVector spawnEnd = spawnStart + direction * EffectiveRange;
			const ECollisionChannel & Channel = UCollisionProfile::Get()->ConvertToCollisionChannel( false, CollisionChannel );
			FHitResult result = Trace( spawnStart, spawnEnd, Channel );

			if ( result.IsValidBlockingHit() )
			{
				spawnEnd = result.ImpactPoint;
				BroadcastHit( result );
			}

			if ( i < ProjectileSources.Num() )
			{
				USceneComponent* source = ProjectileSources[i];
				if ( source )
					source->SetWorldLocation( spawnStart, false, nullptr, ETeleportType::None );
			}

			if ( i < ProjectileTargets.Num() )
			{
				USceneComponent* target = ProjectileTargets[i];
				if ( target )
					target->SetWorldLocation( spawnEnd, false, nullptr, ETeleportType::None );
			}

			if ( i < ColliderSpheres.Num() )
			{
				AActor* sphereActor = ColliderSpheres[i];
				if ( sphereActor )
				{
					sphereActor->SetActorLocation( spawnEnd );
				}
			}

			UPrimitiveComponent* collider = ProjectileColliders[i];
			if ( collider )
				if ( ECollisionEnabled::Type::NoCollision == collider->GetCollisionEnabled() )
					collider->SetCollisionEnabled( ECollisionEnabled::Type::QueryOnly );
		}
	}
}

FHitResult UWeaponType_Beam::Trace( FVector start, FVector end, ECollisionChannel CollisionChannel )
{
	auto owner = GetOwner();
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams( FName( TEXT( "RV_Trace" ) ), true, owner );
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;

	//Re-initialize hit info
	FHitResult RV_Hit( ForceInit );

	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,				//result
		start,				//start
		end,				//end
		CollisionChannel,	//collision channel
		RV_TraceParams
	);

	return RV_Hit;
}


