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
#include "GameCloudSDKUE4/AI/Targeting/AISensorBase.h"
// fixed final include
#include "AITriggerSensor.generated.h"


UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UAITriggerSensor : public UAISensorBase
{
	GENERATED_BODY()

public:

protected:
	//std::vector<UObject*> targets;
	TArray<AActor*> Targets;
	UShapeComponent* Trigger;

public:
	UAITriggerSensor();
	virtual bool IsTargetInRange( AActor* target ) override;

protected:
	virtual void BeginPlay() override;
	virtual TArray<AActor*> GetTargets() override;
	UFUNCTION()
		void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult );
	UFUNCTION()
		void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );	
	UFUNCTION()
		void OnTargetKill( AActor* Target, AController* Killer );
	UFUNCTION ()
		void OnTargetDestroy (AActor* Target);
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|AI|Targeting"
		, Tooltip = "Binds OnOverlapBegin and OnOverlapEnd functions to Trigger" ) )
		void SetupTrigger( UShapeComponent* TriggerToBind );
};
