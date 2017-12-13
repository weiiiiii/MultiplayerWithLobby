// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "VehiclePart.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files

UVehiclePart::UVehiclePart()
	: Type( VehiclePartType::INVALID_PART )
	, SocketName( "" )
{

}

void UVehiclePart::BeginPlay()
{
	Super::BeginPlay();
	
}

TSubclassOf<AActor> FVehiclePartsStruct::GetClassByType( VehiclePartType type )
{
	switch ( type )
	{
		case VehiclePartType::VEHICLE_BUMPER_FRONT:
			return BumperFront;
		case VehiclePartType::VEHICLE_BUMPER_SIDE:
			return BumperSide;
		case VehiclePartType::VEHICLE_BUMPER_BACK:
			return BumperBack;
		case VehiclePartType::VEHICLE_DOOR:
			return Door;
		case VehiclePartType::VEHICLE_SPOILER:
			return Spoiler;
		case VehiclePartType::VEHICLE_WHEEL:
			return Wheel;

		case VehiclePartType::WEAPON_FRONT:
			return WeaponFront;
		case VehiclePartType::WEAPON_FRONT_TOP:
			return WeaponFrontTop;
		case VehiclePartType::WEAPON_FRONT_BOTTOM:
			return WeaponFrontBottom;
		case VehiclePartType::WEAPON_FRONT_SIDE:
			return WeaponFrontSide;
		case VehiclePartType::WEAPON_TOP:
			return WeaponTop;
		case VehiclePartType::WEAPON_BACK:
			return WeaponBack;
		case VehiclePartType::WEAPON_BACK_TOP:
			return WeaponBackTop;
		case VehiclePartType::WEAPON_BACK_BOTTOM:
			return WeaponBackBottom;

		default:
			return nullptr;
	}
}
