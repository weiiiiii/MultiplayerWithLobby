// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "IgnoreInstigatorComponent.generated.h"


UCLASS( ClassGroup=( Custom ), meta=( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UIgnoreInstigatorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
protected:

public:	
	// Sets default values for this component's properties
	UIgnoreInstigatorComponent();
	//UIgnoreInstigatorComponent( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnEndOverlap( class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );	
};
