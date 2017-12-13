// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
// final include
#include "FEntityData.generated.h"

USTRUCT ( BlueprintType )
struct FEntityData
{
	GENERATED_USTRUCT_BODY ()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Data" ) )
		float																Health ;
	UPROPERTY( BlueprintReadOnly, meta = ( Category = "GameCloudSDKUE4|Data" ) )
		bool																IsDead = false;

	FEntityData()
	{

	}
};
