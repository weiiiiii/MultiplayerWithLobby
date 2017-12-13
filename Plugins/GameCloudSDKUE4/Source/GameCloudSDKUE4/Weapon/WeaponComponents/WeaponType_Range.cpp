// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WeaponType_Range.h"
// include STL
// includes Engine files
#include "GameFramework/Pawn.h"
// include plugin files
// includes third party files
// includes project files
#include "SpawnOnShoot.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UWeaponType_Range::UWeaponType_Range()
{

}

bool UWeaponType_Range::GetHasShot()
{
	return HasShot;
}

void UWeaponType_Range::SetupProjectileSpawnPoints( TArray<USceneComponent*> SceneComponents )
{
	this->ProjectileSpawnPoints = SceneComponents;
	for ( int i = 0; i < ProjectileSpawnPoints.Num(); i++ )
	{
		USceneComponent* projectileSpawnPoint = ProjectileSpawnPoints[i];
		FRotator rotation = projectileSpawnPoint->RelativeRotation;
		ProjectileSpawnPointsRotation.Add( rotation );
	}
}

void UWeaponType_Range::ShootStart()
{
	HasShot = true;
	Shoot();
}

void UWeaponType_Range::ShootUpdate()
{
	Shoot();
}

void UWeaponType_Range::ShootEnd()
{
	HasShot = false;
}

FVector UWeaponType_Range::GetPawnVelocity()
{
	FVector toReturn	= FVector::ZeroVector;
	
	AActor* owner		= nullptr;
	AActor* tempOwner	= GetOwner();
	while ( nullptr != tempOwner )
	{
		owner	= tempOwner;
		APawn* pawn	= Cast<APawn>( owner );
		if ( nullptr != pawn )
		{
			toReturn	= pawn->GetVelocity();
			UCommonFunctions::PrintToLog( UKismetSystemLibrary::GetDisplayName( pawn ) + " found, PAWN!!!" );
			break;
		}
		else
		{
			UCommonFunctions::PrintToLog( UKismetSystemLibrary::GetDisplayName( owner ) + " found, NOT A PAWN!" );
		}
		tempOwner	= owner->GetOwner();
	}

	UCommonFunctions::PrintToLog( "Returning " + toReturn.ToCompactString() );
	return toReturn;
}

void UWeaponType_Range::RotateProjectileSpawnPoints()
{
	for ( int i = 0; i < ProjectileSpawnPoints.Num(); i++ )
	{
		if ( ProjectileSpawnPointsRotation.Num() > i )
		{
			FRotator originalRotation = ProjectileSpawnPointsRotation[i];
			float yawRange = UKismetMathLibrary::RandomFloatInRange( -WeaponSpread.Yaw, WeaponSpread.Yaw );
			float pitchRange = UKismetMathLibrary::RandomFloatInRange( -WeaponSpread.Pitch, WeaponSpread.Pitch );
			FRotator randomRotation = UKismetMathLibrary::MakeRotator( 0.0f, pitchRange, yawRange );
			FRotator newRotation = originalRotation + randomRotation;

			USceneComponent* projectileSpawnPoint = ProjectileSpawnPoints[i];			
			projectileSpawnPoint->SetRelativeRotation( newRotation.Quaternion() );
		}
	}
}

void UWeaponType_Range::Shoot()
{
	RotateProjectileSpawnPoints();
	for ( auto spawnPoint : EffectSpawnPoints )
	{
		USpawnOnShoot* fxSpawnPoint = Cast<USpawnOnShoot>( spawnPoint );
		if ( fxSpawnPoint )
			fxSpawnPoint->Spawn();
	}
}
