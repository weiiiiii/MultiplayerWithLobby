// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <unordered_map>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "EntityPlaceholderComponent.generated.h"

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), Blueprintable, meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UEntityPlaceholderComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/**	The name of socket that the spawned actor is to be attached to. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, meta = ( Category = "Entity" ) )
		FName SocketName;
	/**	The type of entity part this placeholder is representing. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, meta = ( Category = "Entity" ) )
		FString TypeString;

protected:

public:
	UEntityPlaceholderComponent( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );

protected:
	virtual void BeginPlay() override;
		
	
};
