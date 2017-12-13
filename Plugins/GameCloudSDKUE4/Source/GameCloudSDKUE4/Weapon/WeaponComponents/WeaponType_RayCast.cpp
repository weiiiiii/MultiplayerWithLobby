// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WeaponType_RayCast.h"
// include STL
// includes Engine files
//#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
// include plugin files
// includes third party files
// includes project files

void UWeaponType_RayCast::Shoot()
{
	if ( NULL != ProjectileType )
	{
		UWeaponType_Range::Shoot();

		AActor* owner = GetOwner();
		check( owner != nullptr );

		auto world = owner->GetWorld();
		check( world != nullptr );

		for ( auto spawnPoint : ProjectileSpawnPoints )
		{
			if ( NULL == spawnPoint )
				continue;

			FVector spawnStart = spawnPoint->GetComponentLocation();
			FVector direction = spawnPoint->GetForwardVector();
			FVector spwanEnd = spawnStart + direction * EffectiveRange;
			FHitResult result = Trace( spawnStart, spwanEnd );

			if ( !result.IsValidBlockingHit() )
				continue;

			FRotator spawnRotation = spawnPoint->GetComponentRotation();
			AActor* projectile = owner->GetWorld()->SpawnActor<AActor>( ProjectileType, result.ImpactPoint, spawnRotation );
			projectile->Instigator = owner->GetRootComponent()->GetAttachmentRootActor()->GetInstigator();
		}

		/*DrawDebugLine(
		GetWorld(),
		spawnStart,
		spwanEnd,
		FColor( 255, 0, 0 ),
		false, 10, 0,
		12.333
		);*/
	}	
}

FHitResult UWeaponType_RayCast::Trace( FVector start, FVector end )
{
	auto owner = GetOwner();
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams( FName( TEXT( "RV_Trace" ) ), true, owner );
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	const ECollisionChannel & Channel = UCollisionProfile::Get()->ConvertToCollisionChannel( false, CollisionChannel );

	//Re-initialize hit info
	FHitResult RV_Hit( ForceInit );

	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,        //result
		start,    //start
		end, //end
		Channel, //collision channel
		RV_TraceParams
	);

	return RV_Hit;
}
