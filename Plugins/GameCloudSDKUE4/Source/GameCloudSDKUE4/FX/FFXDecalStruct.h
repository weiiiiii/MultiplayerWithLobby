// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "Sound/SoundCue.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "FFXDecalStruct.generated.h"

USTRUCT ( BlueprintType )
struct FFXDecalStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY ()

	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		TEnumAsByte<EPhysicalSurface> Surface;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		UParticleSystem* ImpactParticleFX;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector ImpactParticleScale;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		USoundCue* ImpactSoundFX;
	UPROPERTY (EditAnywhere, BlueprintReadWrite)
		float VolumeMultiplier_;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		UMaterial* ImpactDecal;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		FVector ImpactDecalSize;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		float ImpactDecalSizeRandomMin;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float ImpactDecalSizeRandomMax;
//  	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//  		bool ImpactDecalIsAttached;
 	UPROPERTY( EditAnywhere, BlueprintReadWrite )
 		float ImpactDecalLifespan;
 	UPROPERTY( EditAnywhere, BlueprintReadWrite )
 		float ImpactDecalFadeScreenSize;
	UPROPERTY ( EditAnywhere, BlueprintReadWrite )
		UParticleSystem* DebrisParticleFX;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector DebrisParticleScale;

	FFXDecalStruct()
		: Surface( EPhysicalSurface::SurfaceType_Default )
		, ImpactParticleFX( nullptr )
		, ImpactParticleScale( FVector::OneVector )
		, ImpactSoundFX( nullptr )
		, VolumeMultiplier_( 1.0f )
		, ImpactDecal( nullptr )
		, ImpactDecalSize( FVector::OneVector )
		, ImpactDecalSizeRandomMin( 1.0f )
		, ImpactDecalSizeRandomMax( 1.0f )
 		//, ImpactDecalIsAttached( true )
 		, ImpactDecalLifespan( 20.0f )
		, ImpactDecalFadeScreenSize( 0.01 )
		, DebrisParticleFX( nullptr )
		, DebrisParticleScale( FVector::OneVector )
	{

	}


	void SpawnDebrisParticleFX( UObject* world, const FHitResult& Hit, FVector scale = FVector( 1.0f ), float impactSpeed = 1.0f, float impactForceMultiplier = 1.0f )
	{
		SpawnParticleFX( DebrisParticleFX, DebrisParticleScale, world, Hit, scale, impactSpeed, impactForceMultiplier );
	}
	
	void SpawnImpactParticleFX( UObject* world, const FHitResult& Hit, FVector scale = FVector( 1.0f ), float impactSpeed = 1.0f, float impactForceMultiplier = 1.0f )
	{
		SpawnParticleFX( ImpactParticleFX, ImpactParticleScale, world, Hit, scale, impactSpeed, impactForceMultiplier );
	}

	void SpawnImpactDecals( UObject* world, const FHitResult& Hit, FVector scale = FVector( 1.0f ) )
	{
		if ( !world )
			return;

		UMaterialInstanceDynamic* newMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance( world, Cast<UMaterialInterface>( ImpactDecal ) );
		if ( newMat )
		{
			newMat->SetScalarParameterValue( FName( "Frame" ), UKismetMathLibrary::RandomFloatInRange( 0, 3.0f ) );

			FRotator rotation = UKismetMathLibrary::MakeRotFromX( Hit.ImpactNormal );
			rotation.Roll = UKismetMathLibrary::RandomFloatInRange( 0.0f, 360.0f );
			FVector decalSize = scale * ImpactDecalSize * UKismetMathLibrary::RandomFloatInRange( ImpactDecalSizeRandomMin, ImpactDecalSizeRandomMax );

			UDecalComponent* decalComponent = nullptr;
			if ( true )//ImpactDecalIsAttached )
			{
				UPrimitiveComponent* component = Hit.GetComponent();
				if ( component )
					decalComponent = UGameplayStatics::SpawnDecalAttached( newMat, decalSize, component, Hit.BoneName, Hit.ImpactPoint, rotation, EAttachLocation::KeepWorldPosition, ImpactDecalLifespan );
			}
			else
				if ( world )
					decalComponent = UGameplayStatics::SpawnDecalAtLocation( world, newMat, decalSize, Hit.ImpactPoint, rotation, ImpactDecalLifespan );

			if ( decalComponent )
			{
				decalComponent->SetFadeOut( ImpactDecalLifespan, 5.0f, false );
				decalComponent->FadeScreenSize = ImpactDecalFadeScreenSize;
			}
		}
	}

	void PlayImpactSound( UObject* world, const FHitResult& Hit )
	{
		if ( !ImpactSoundFX )
			return;
		if ( !world )
			return;

		UGameplayStatics::SpawnSoundAtLocation( world, ImpactSoundFX, Hit.ImpactPoint, FRotator::ZeroRotator, VolumeMultiplier_, UGameplayStatics::GetGlobalTimeDilation( world ) );
	}

protected:
	void SpawnParticleFX( UParticleSystem* particleSystem, FVector particleScale, UObject* world, const FHitResult& Hit, FVector scale = FVector( 1.0f ), float impactSpeed = 1.0f, float impactForceMultiplier = 1.0f )
	{
		if ( !world )
			return;
		if ( !particleSystem )
			return;

		FRotator rotation = UKismetMathLibrary::MakeRotFromZ( Hit.ImpactNormal );
		UParticleSystemComponent* particle = UGameplayStatics::SpawnEmitterAtLocation( world, particleSystem, Hit.ImpactPoint, rotation );

		// set scale (Cubit_ImpactFX_Spawner > Impact Particle FX)
		FVector finalScale = scale * impactSpeed * impactForceMultiplier * particleScale;
		particle->SetRelativeScale3D( finalScale );
	}
};
