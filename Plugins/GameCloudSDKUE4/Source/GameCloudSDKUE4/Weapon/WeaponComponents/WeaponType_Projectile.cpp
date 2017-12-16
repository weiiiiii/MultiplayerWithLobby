// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WeaponType_Projectile.h"
// include STL
// includes Engine files
#include "Components/ShapeComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "Kismet/KismetSystemLibrary.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Weapon/BaseProjectile_CPP.h"

void UWeaponType_Projectile::Shoot()
{
	UWeaponType_Range::Shoot();

	if ( NULL == ProjectileType )
	{ return; }

	AActor* owner		= GetOwner ();
	check ( owner != nullptr );

	auto world			= owner->GetWorld ();
	check ( world != nullptr );	

	FVector pawnVelocity	= GetPawnVelocity();

	APawn* instigator				= owner->GetRootComponent()->GetAttachmentRootActor()->GetInstigator();
	FActorSpawnParameters params	= FActorSpawnParameters();
	params.Instigator				= instigator;
	params.Owner					= owner;
	for ( auto spawnPoint : ProjectileSpawnPoints )
	{
		if ( NULL == spawnPoint )
		{ continue; }

		FVector spawnStart				= spawnPoint->GetComponentLocation ();
		FRotator spawnRotation			= spawnPoint->GetComponentRotation ();
		ABaseProjectile_CPP* projectile	= owner->GetWorld ()->SpawnActor<ABaseProjectile_CPP> ( ProjectileType, spawnStart, spawnRotation, params );
		if ( projectile )
		{
 			UShapeComponent* projectileCollider	= Cast<UShapeComponent>( projectile->GetComponentByClass( UShapeComponent::StaticClass() ) );
 			if ( projectileCollider )
 				projectileCollider->IgnoreActorWhenMoving( projectile->Instigator, true );
			//UCommonFunctions::PrintToLog( "Ignoring " + UKismetSystemLibrary::GetDisplayName( projectile->Instigator ) );
			UProjectileMovementComponent* projectileMovementComponent	= Cast<UProjectileMovementComponent>( projectile->GetComponentByClass( UProjectileMovementComponent::StaticClass() ) );
			//{ UCommonFunctions::PrintToLog( "Old velocity : " + projectile->GetVelocity().ToCompactString() ); }
			projectileMovementComponent->Velocity	+= pawnVelocity;
			projectileMovementComponent->UpdateComponentVelocity();
			//{ UCommonFunctions::PrintToLog( "Old velocity : " + projectile->GetVelocity().ToCompactString() ); }
		}
	}
}
