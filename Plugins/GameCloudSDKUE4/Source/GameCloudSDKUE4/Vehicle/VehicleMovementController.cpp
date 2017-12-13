// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "VehicleMovementController.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files


// Sets default values for this component's properties
UVehicleMovementController::UVehicleMovementController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleMovementController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVehicleMovementController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

