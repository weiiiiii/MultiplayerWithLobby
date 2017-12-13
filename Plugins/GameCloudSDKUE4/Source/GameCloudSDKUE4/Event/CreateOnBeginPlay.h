// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "SpawnOnEvent.h"
// fixed final include
#include "CreateOnBeginPlay.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UCreateOnBeginPlay : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKEvent|On Component Hit"
		, Tooltip = "SoundBases, actors and/or particles to spawn on event BeginPlay" ) )
		TArray <FSpawnOnEvent> SpawnOnEvent;

public:
	UCreateOnBeginPlay();
	
protected:
	virtual void BeginPlay() override;	
};
