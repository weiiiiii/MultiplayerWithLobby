// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <unordered_map>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "VehiclePart.generated.h"

UENUM( BlueprintType )
enum class VehiclePartType : uint8
{
	VEHICLE_BUMPER_FRONT		UMETA( DisplayName = "Vehicle Bumper Front" ),
	VEHICLE_BUMPER_SIDE			UMETA( DisplayName = "Vehicle Bumper Side" ),
	VEHICLE_BUMPER_BACK			UMETA( DisplayName = "Vehicle Bumper Back" ),
	VEHICLE_DOOR				UMETA( DisplayName = "Vehicle Door" ),
	VEHICLE_SPOILER				UMETA( DisplayName = "Vehicle Spoiler" ),
	VEHICLE_WHEEL				UMETA( DisplayName = "Vehicle Wheel" ),

	WEAPON_FRONT				UMETA( DisplayName = "Weapon Front" ),
	WEAPON_FRONT_TOP			UMETA( DisplayName = "Weapon Front Top" ),
	WEAPON_FRONT_BOTTOM			UMETA( DisplayName = "Weapon Front Bottom" ),
	WEAPON_FRONT_SIDE			UMETA( DisplayName = "Weapon Front Side" ),
	WEAPON_TOP					UMETA( DisplayName = "Weapon Top" ),
	WEAPON_BACK					UMETA( DisplayName = "Weapon Back" ),
	WEAPON_BACK_TOP				UMETA( DisplayName = "Weapon Back Top" ),
	WEAPON_BACK_BOTTOM			UMETA( DisplayName = "Weapon Back Bottom" ),

	INVALID_PART				UMETA( DisplayName = "Invalid" )//,
	//FIRST						= VEHICLE_BUMPER_FRONT,
	//LAST						= WEAPON_BACK_BOTTOM
};

USTRUCT()
struct FVehiclePartsStruct : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> BumperFront;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> BumperSide;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> BumperBack;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> Door;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> Spoiler;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> Wheel;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponFront;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponFrontTop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponFrontBottom;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponFrontSide;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponTop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponBack;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponBackTop;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TSubclassOf<AActor> WeaponBackBottom;

	FVehiclePartsStruct()
		: BumperFront( nullptr )
		, BumperSide( nullptr )
		, BumperBack( nullptr )
		, Door( nullptr )
		, Spoiler( nullptr )
		, Wheel( nullptr )

		, WeaponFront( nullptr )
		, WeaponFrontTop( nullptr )
		, WeaponFrontBottom( nullptr )
		, WeaponFrontSide( nullptr )
		, WeaponTop( nullptr )
		, WeaponBack( nullptr )
		, WeaponBackTop( nullptr )
		, WeaponBackBottom( nullptr )
	{

	}

public:
	TSubclassOf<AActor> GetClassByType( VehiclePartType type );
};


UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UVehiclePart : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		VehiclePartType Type;
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		FName SocketName;

	UVehiclePart();
protected:
	void BeginPlay();		
	
};
