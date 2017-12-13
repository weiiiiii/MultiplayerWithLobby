// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "VehicleMovementController.generated.h"


UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UVehicleMovementController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleMovementController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
