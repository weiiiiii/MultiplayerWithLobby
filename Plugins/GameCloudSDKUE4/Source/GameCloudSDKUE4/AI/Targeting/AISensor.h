// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/AI/Targeting/AISensorBase.h"
// fixed final include
#include "AISensor.generated.h"


UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UAISensor : public UAISensorBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|AI|Targeting" ) )
		float Range_;

protected:

public:	
	UAISensor();
	virtual bool IsTargetInRange( AActor* target ) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void FilterTargets( TArray<AActor*>& targets );
	virtual TArray<AActor*> GetTargets() override;
};
