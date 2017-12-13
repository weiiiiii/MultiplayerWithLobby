// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/SceneComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "ParticleTrail.generated.h"

USTRUCT( BlueprintType )
struct FParticleTrailStruct
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Weapon|ProjectileComponents"
		, Tooltip = "Particle system to spawn" ) )
		UParticleSystem* ParticleSystem;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( category = "GameCloudSDKUE4|Weapon|ProjectileComponents"
		, Tooltip = "Scale of particle system to spawn" ) )
		FVector Scale;

	FParticleTrailStruct( UParticleSystem* ParticleSystem = nullptr, FVector Scale = FVector( 1.0f ) )
		: ParticleSystem( ParticleSystem )
		, Scale( FVector( Scale ) )
	{

	}
};

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UParticleTrail : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|ProjectileComponents"
		, Tooltip = "Particle trails to spawn" ) )
		TArray<FParticleTrailStruct> ParticleTrailsToSpawn;

protected:
	TArray<UParticleSystemComponent*> particlesSpawned;
	
public:
	UParticleTrail();
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Weapon|ProjectileComponents",
		ToolTip = "Deactivates all particle system components in particle trail" ) )
	bool DeactivateTrails( bool destroyOwner = true );
	
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		void removeParticleSystem( UParticleSystemComponent* psc );
	UFUNCTION()
		void removeParticleSystemAndDestroyOwner( UParticleSystemComponent* psc );
};
