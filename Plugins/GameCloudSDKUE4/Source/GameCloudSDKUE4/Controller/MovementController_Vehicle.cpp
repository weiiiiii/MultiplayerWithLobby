// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "MovementController_Vehicle.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files

void UMovementController_Vehicle::MoveForward( float value )
{
	//float clampedValue = UKismetMathLibrary::FClamp( value, 0.0f, 1.0f );
	if ( value <= 0 )
		return;

	xInput = value;
	isMovingForward = value > 0;
}

void UMovementController_Vehicle::MoveBackward( float value )
{
	if ( value >= 0 )
		return;

	xInput = value;
	isMovingForward = value < 0;
}

void UMovementController_Vehicle::TurnLeft( float value )
{

}

void UMovementController_Vehicle::TurnRight( float value )
{

}

void UMovementController_Vehicle::BeginPlay()
{
	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

// 	AWheeledVehicle* vehicle = Cast <AWheeledVehicle>( owner );
// 	if ( nullptr == vehicle )
}
