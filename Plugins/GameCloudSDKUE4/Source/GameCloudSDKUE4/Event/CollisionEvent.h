// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "SpawnOnEvent.h"
#include "DebugOnEvent.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
// fixed final include
#include "CollisionEvent.generated.h"

class UDamageType;

USTRUCT( BlueprintType )
struct FApplyDamageStruct
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool IsEnabled;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float BaseDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<UDamageType> DamageType;

	FApplyDamageStruct()
		: IsEnabled( false )
		, BaseDamage( 0.0f )
		, DamageType( nullptr )
	{

	}
};

USTRUCT( BlueprintType )
struct FApplyRadialDamageStruct
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool IsEnabled;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float BaseDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float DamageRadius;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool DoFullDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<UDamageType> DamageType;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TEnumAsByte<ECollisionChannel> DamagePreventionChannel;

	FApplyRadialDamageStruct()
		: IsEnabled( false )
		, BaseDamage( 0.0f )
		, DamageRadius( 0.0f )
		, DoFullDamage( false )
		, DamageType( nullptr )
		, DamagePreventionChannel()
	{

	}
};

USTRUCT( BlueprintType )
struct FApplyRadialDamageWithFalloffStruct
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool IsEnabled;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float BaseDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float MinimumDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float DamageInnerRadius;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float DamageOuterRadius;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float DamageFalloff;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<UDamageType> DamageType;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TEnumAsByte<ECollisionChannel> DamagePreventionChannel;

	FApplyRadialDamageWithFalloffStruct()
		: IsEnabled( false )
		, BaseDamage( 0.0f )
		, MinimumDamage( 0.0f )
		, DamageInnerRadius( 0.0f )
		, DamageOuterRadius( 0.0f )
		, DamageFalloff( 0.0f )
		, DamageType( nullptr )
		, DamagePreventionChannel()
	{

	}
};

USTRUCT( BlueprintType )
struct FDebugDamageStruct
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool PrintOnScreen;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FColor ScreenColour;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float Duration;

	FDebugDamageStruct()
		: PrintOnScreen( false )
		, ScreenColour( FColor::Red )
		, Duration( 2.0f )
	{

	}

	void PrintOnEvent( AActor* actor, FHitResult Hit )
	{
		FString message = "";
		if ( nullptr != actor )
		{
			FString actorName = UKismetSystemLibrary::GetDisplayName( Cast<UObject>( actor ) );
			message += actorName;
		}
		else
			message += "Nameless";

		message += " dealt damage to ";

		if ( Hit.GetActor() )
		{
			FString actorName = UKismetSystemLibrary::GetDisplayName( Cast<UObject>( Hit.GetActor() ) );
			message += actorName;
		}

		if ( PrintOnScreen )
			UCommonFunctions::PrintFStringOnScreen( Duration, ScreenColour, message );
		// 		if ( PrintInLog )
		// 			UE_LOG( LogTemp, Warning, TEXT( "%s" ),  message );
	}
};

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UCollisionEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	//UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "When enabled, component will ingore instigator until it has stop overlapping instigator" ) )
	//	bool IgnoreInstigatorOnStart;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event"
		, Tooltip = "Time taken for sound to fade to a stop when owner is destroyed" ) )
		float SoundFadeOutDuration;

	//////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "When enabled, component will apply damage on event, OnComponentHit, of ShapeComponent owned by owner" ) )
		bool DamageOnHit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "When enabled, component will apply damage on event, OnComponentOverlapBegin, of ShapeComponent owned by owner" ) )
		bool DamageOnBeginOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "TBC" ) )
		bool DamageOnOverlapUpdate;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "TBC" ) )
		bool DamageOnEndOverlap;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "Values to use when calling Apply Damage" ) )
		FApplyDamageStruct ApplyDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "Values to use when calling Apply Point Damage" ) )
		FApplyDamageStruct ApplyPointDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "Values to use when calling Apply Radial Damage" ) )
		FApplyRadialDamageStruct ApplyRadialDamage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|Damage", Tooltip = "Values to use when calling Apply Radial Damage With Falloff" ) )
		FApplyRadialDamageWithFalloffStruct ApplyRadialDamageWithFalloff;

	//////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Component Hit"
		, Tooltip = "Enable to allow spawning, creating decals, destroy owning actor and debug on event OnComponentHit" ) )
		bool EnableOnHit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Component Hit"
		, Tooltip = "SoundBases, actors and/or particles to spawn on event OnComponentHit" ) )
		TArray <FSpawnOnEvent> SpawnOnHit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Component Hit"
		, Tooltip = "Table of surfaces and decal materials to spawn on event OnComponentHit" ) )
		UDataTable* DecalOnHit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Component Hit"
		, Tooltip = "Destroy actor on event OnComponentHit. Sound will fade out and destroy if owner does not have any particle trail(s). If it has at least one, mesh of owner will be hidden with collision disabled and owner will destroy once all particles are destroyed." ) )
		bool DestroyActorOnHit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Component Hit"
		, Tooltip = "Enables debug on event OnComponentHit" ) )
		FDebugOnEvent DebugOnHit;
	
	//////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Overlap"
		, Tooltip = "Table of surfaces and decal materials to spawn on event OnComponentOverlap" ) )
		UDataTable* DecalOnOverlap;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Begin Overlap"
		, Tooltip = "Enable to allow spawning, creating decals, destroy owning actor and debug on event OnComponentOverlapBegin" ) )
		bool EnableOnBeginOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Begin Overlap"
		, Tooltip = "SoundBases, actors and/or particles to spawn on event OnComponentOverlapBegin" ) )
		TArray <FSpawnOnEvent> SpawnOnBeginOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Begin Overlap"
		, Tooltip = "Enables spawning of decal on event OnComponentOverlapBegin" ) )
		bool SpawnDecalOnBeginOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Begin Overlap"
		, Tooltip = "Destroy actor on event OnComponentOverlapBegin. Sound will fade out and destroy if owner does not have any particle trail(s). If it has at least one, mesh of owner will be hidden with collision disabled and owner will destroy once all particles are destroyed." ) )
		bool DestroyActorOnBeginOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Begin Overlap"
		, Tooltip = "Enables debug on event OnComponentOverlapBegin" ) )
		FDebugOnEvent DebugOnBeginOverlap;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Overlap Update"
		, Tooltip = "Enable to allow spawning, creating decals, destroy owning actor and debug on event OnComponentOverlapUpdate" ) )
		bool EnableOnOverlapUpdate;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Overlap Update"
		, Tooltip = "SoundBases, actors and/or particles to spawn on event OnComponentOverlapUpdate" ) )
		TArray <FSpawnOnEvent> SpawnOnOverlapUpdate;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Overlap Update"
		, Tooltip = "Enables spawning of decal on event OnComponentOverlapUpdate" ) )
		bool SpawnDecalOnOverlapUpdate;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Overlap Update"
		, Tooltip = "Destroy actor on event OnComponentOverlapUpdate. Sound will fade out and destroy if owner does not have any particle trail(s). If it has at least one, mesh of owner will be hidden with collision disabled and owner will destroy once all particles are destroyed." ) )
		bool DestroyActorOnOverlapUpdate;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On Overlap Update"
		, Tooltip = "Enables debug on event OnComponentOverlapUpdate" ) )
		FDebugOnEvent DebugOnOverlapUpdate;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On End Overlap"
		, Tooltip = "Enable to allow spawning, creating decals, destroy owning actor and debug on event OnComponentEndOverlap" ) )
		bool EnableOnEndOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On End Overlap"
		, Tooltip = "Destroy actor on event OnComponentEndOverlap. Sound will fade out and destroy if owner does not have any particle trail(s). If it has at least one, mesh of owner will be hidden with collision disabled and owner will destroy once all particles are destroyed." ) )
		bool DestroyActorOnEndOverlap;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event|On End Overlap"
		, Tooltip = "Enables debug on event OnComponentEndOverlap" ) )
		FDebugOnEvent DebugOnEndOverlap;

protected:
	UShapeComponent* shapeComponent;
	UAudioComponent* audioComponent;
	bool ShouldDestroy;
		
public:
	UCollisionEvent();
	UFUNCTION()
		void OnHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit );
	UFUNCTION()
		void OnBeginOverlap( class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
		void OnOverlapUpdate();
	UFUNCTION()
		void OnEndOverlap( class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
	void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	void BeginPlay() override;
	UFUNCTION()
		void ApplyAllDamage( FHitResult Hit, AController* EventInstigatorController, AActor* DamageCauser );

	void spawnDecal( UDataTable* decalTable, FHitResult hitResult );
	void destroyActor();
};
