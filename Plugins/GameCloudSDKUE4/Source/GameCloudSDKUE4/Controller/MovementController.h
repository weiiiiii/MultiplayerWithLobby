// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
// includes project files
// include plugin files
// includes third party files
// fixed final include
#include "MovementController.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UMovementController : public UActorComponent
{
	GENERATED_BODY()

protected:
	float xInput;
	float yInput;
	bool isMovingForward;

public:	
	UMovementController();
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller" ) )
		virtual void MoveForward( float value ) {}
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller" ) )
		virtual void MoveBackward( float value ) {}
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller" ) )
		virtual void TurnLeft( float value ) {}
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller" ) )
		virtual void TurnRight( float value ) {}
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller" ) )
		virtual void StrafeLeft( float value ) {}
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller" ) )
		virtual void StrafeRight( float value ) {}
	
};
