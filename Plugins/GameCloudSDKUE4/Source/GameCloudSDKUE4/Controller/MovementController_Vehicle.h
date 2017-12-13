// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
//#include "Public/WheeledVehicleMovementComponent4W.h"
//#include "WheeledVehicleMovementComponent4W.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Controller/MovementController.h"
// fixed final include
#include "MovementController_Vehicle.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UMovementController_Vehicle : public UMovementController
{
	GENERATED_BODY()
		
public:
	// UWheeledVehicleMovementComponent and UWheeledVehicleMovementComponent4W cannot be declared as UPROPERTY!!!!
	// will result in Unrecognized type 'UWheeledVehicleMovementComponent4W' - type must be a UCLASS, USTRUCT or UENUM
	//UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloud" ) )
	//UPROPERTY( BlueprintReadWrite, meta = ( Category = "GameCloud" ) )
	//UWheeledVehicleMovementComponent4W* MovementComponent;
	//UMovementComponent* MovementComponent;
	//class UWheeledVehicleMovementComponent* MovementComponent;
	
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Controller" ) )
		virtual void MoveForward( float value ) override;
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Controller" ) )
		virtual void MoveBackward( float value ) override;
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Controller" ) )
		virtual void TurnLeft( float value ) override;
	UFUNCTION( BlueprintCallable, meta = ( category = "GameCloudSDKUE4|Controller" ) )
		virtual void TurnRight( float value ) override;	

protected:
	void BeginPlay() override;
};
