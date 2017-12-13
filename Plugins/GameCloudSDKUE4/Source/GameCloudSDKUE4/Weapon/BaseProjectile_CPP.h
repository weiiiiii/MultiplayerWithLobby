// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "GameFramework/Actor.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "BaseProjectile_CPP.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ) )
class GAMECLOUDSDKUE4_API ABaseProjectile_CPP : public AActor
{
	GENERATED_BODY()
	
public:
	ABaseProjectile_CPP();

protected:
	virtual void BeginPlay() override;
	void setInstigatorFromParent();
};
