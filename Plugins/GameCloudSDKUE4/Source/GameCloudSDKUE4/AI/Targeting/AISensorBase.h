// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <vector>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Entity/EntityComponent.h"
// fixed final include
#include "AISensorBase.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UAISensorBase : public UActorComponent
{
	GENERATED_BODY()

public:

protected:

public:
	UAISensorBase();
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, Tooltip = "Get all targets within range" ) )
		virtual TArray<AActor*> GetTargets();
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, Tooltip = "Checks if given target is in range" ) )
		virtual bool IsTargetInRange( AActor* target );

protected:
	virtual void BeginPlay() override;
	bool IsValidTarget( AActor* target );
	AActor* GetBaseActor( AActor* target );
};
