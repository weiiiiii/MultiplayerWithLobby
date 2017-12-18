// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <vector>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "AITargetLogic.generated.h"

class AActor;
class UAISensorBase;
class UWeaponLogic;
//class ABaseWeapon_CPP;

UENUM( BlueprintType )
enum class SortTargetBy : uint8
{
	Closest,
	Furthest,
	Highest_HP,
	Lowest_HP
};

UENUM( BlueprintType )
enum class TargetingLogic : uint8
{
	Till_Death,
	Fixed_Update
};

UENUM( BlueprintType )
enum class RotationType : uint8
{
	Roll,
	Pitch,
	Yaw
};

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UAITargetLogic : public UActorComponent
{
	GENERATED_BODY()
		
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Algorithm to use when getting target(s)" ) )
		SortTargetBy														SortType;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "When a new target should be acquired" ) )
		TargetingLogic														TargetLogic;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Minimum angle to rotate, from -180 to 180" ) )
		FRotator															RotationMinumum;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Maximum angle to rotate, from -180 to 180" ) )
		FRotator															RotationMaximum;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Speed at which the scene component rotates" ) )
		FRotator															AngularVelocity;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Time to wait if a target was not found in current FindTarget() call" ) )
		float																SenseCooldown;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Rotates all RotationComponents back to starting rotation if set to true" ) )
		bool																ShouldRotateToStartRotation;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Angle from forward vector for target to be considered in range" ) )
		float																ShootAnglePitch;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Angle from forward vector for target to be considered in range" ) )
		float																ShootAngleYaw;


	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "AISensorBase owned by pawn that owns parent" ) )
		UAISensorBase*														SensorBase;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "The scene component to rotate for Roll(X-Axis)" ) )
		USceneComponent*													RollComponent;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "The scene component to rotate for Pitch(Y-Axis)" ) )
		USceneComponent*													PitchComponent;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "The scene component to rotate for Yaw(Z-Axis)" ) )
		USceneComponent*													YawComponent;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Target found through SensorBase" ) )
		AActor*																Target;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "The scene component from which attack starts (projectile, beam, etc)" ) )
		USceneComponent*													WeaponComponent;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "WeaponLogic owned by parent (may change to WeaponData to get number of targets)" ) )
		UWeaponLogic*														WeaponLogic;
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, ToolTip = "Scene components that represent start of projectile / beam" ) )
		USceneComponent*													AttackSpawnComponent;
		//TArray<USceneComponent*>											AttackSpawnComponents;
	//UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
	//	, ToolTip = "Targets found through SensorBase" ) )
	//	TArray<AActor*> Targets_;

protected:

	float																	Cooldown_;
	std::vector<FRotator>													StartingRotators;

public:
	UAITargetLogic();
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, Tooltip = "Returns true if target is within shoot range" ) )
		bool IsTargetInShootingRange();

protected:
	virtual void BeginPlay() override;
	void FindTarget();
	void Rotate( float DeltaTime );
	void RotateComponent( FRotator TargetRotation, FRotator CurrentRotation, USceneComponent* Component, float DeltaTime, RotationType Type );
	void RotateComponent2( FRotator DiffInRotation, USceneComponent* Component, float DeltaTime, RotationType Type );
	void SetStartingRotator( USceneComponent* Component );

	UFUNCTION()
		void OnTargetDeath( AActor* KilledActor, AController* KilledBy );
	UFUNCTION ()
		void OnTargetDestroyed (AActor* DestroyedActor);
};
