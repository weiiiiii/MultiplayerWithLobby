// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "SpawnOnEvent.h"
// final include
#include "AutoDestroyEvent.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UAutoDestroyEvent : public UActorComponent
{
	GENERATED_BODY()
				
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event"
		, ToolTip = "Time to delay before destroying owner of this component, does not destroy if LifeSpan is smaller or equals to 0" ) )
		float LifeSpan;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event"
		, Tooltip = "SoundBases, actors and/or particles to spawn when Life Span is up" ) )
		TArray <FSpawnOnEvent> SpawnOnAutoDestroy;
		
protected:
	/* Handle to manage the timer */
	FTimerHandle lifespanHandle;

public:
	UAutoDestroyEvent();
	//void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	void BeginPlay() override;
	void setLifespanTimer();
	void lifeOver();
	void destroyActor();
};
