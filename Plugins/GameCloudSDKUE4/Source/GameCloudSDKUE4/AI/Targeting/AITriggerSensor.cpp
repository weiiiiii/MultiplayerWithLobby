// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "AITriggerSensor.h"
// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Engine/World.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Entity/EntityComponent.h"

UAITriggerSensor::UAITriggerSensor()
	: Targets()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UAITriggerSensor::IsTargetInRange( AActor* Target )
{
	return Targets.Contains( Target );
}

void UAITriggerSensor::BeginPlay()
{
	Super::BeginPlay();
}

TArray<AActor*> UAITriggerSensor::GetTargets()
{
	TArray<AActor*> toReturn;
	if ( ensureAlwaysMsgf( nullptr!= Trigger, *FString( "Trigger was not set up!" ) ) )
	{ toReturn = Targets; }
	return toReturn;
}

void UAITriggerSensor::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult )
{
	if ( IsValidTarget( OtherActor ) )
	{
		Targets.Push( OtherActor );
		UEntityComponent* entityComponent = Cast<UEntityComponent>( OtherActor->GetComponentByClass( UEntityComponent::StaticClass() ) );
		if ( !entityComponent->OnEntityKill.IsAlreadyBound( this, &UAITriggerSensor::OnTargetKill ) )
		{ entityComponent->OnEntityKill.AddDynamic( this, &UAITriggerSensor::OnTargetKill ); }
		if ( !entityComponent->OnEntityDestroy.IsAlreadyBound( this, &UAITriggerSensor::OnTargetDestroy ) )
		{ entityComponent->OnEntityDestroy.AddDynamic( this, &UAITriggerSensor::OnTargetDestroy); }
	}
}

void UAITriggerSensor::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	if ( IsValidTarget( OtherActor ) )
	{ 
		Targets.Remove( OtherActor );
		UEntityComponent* entityComponent = Cast<UEntityComponent>( OtherActor->GetComponentByClass( UEntityComponent::StaticClass() ) );
		entityComponent->OnEntityKill.RemoveDynamic( this, &UAITriggerSensor::OnTargetKill );
		entityComponent->OnEntityDestroy.RemoveDynamic( this, &UAITriggerSensor::OnTargetDestroy);
	}
}

void UAITriggerSensor::SetupTrigger( UShapeComponent* TriggerToBind )
{
	this->Trigger = TriggerToBind;
	if ( !Trigger->OnComponentBeginOverlap.IsAlreadyBound( this, &UAITriggerSensor::OnOverlapBegin ) )
	{ Trigger->OnComponentBeginOverlap.AddDynamic( this, &UAITriggerSensor::OnOverlapBegin ); }
	if ( !Trigger->OnComponentEndOverlap.IsAlreadyBound( this, &UAITriggerSensor::OnOverlapEnd ) )
	{ Trigger->OnComponentEndOverlap.AddDynamic( this, &UAITriggerSensor::OnOverlapEnd ); }
}

void UAITriggerSensor::OnTargetKill( AActor* Target, AController* Killer )
{
	if ( Targets.Contains( Target ) )
	{ Targets.Remove( Target ); }
}

void UAITriggerSensor::OnTargetDestroy (AActor* Target)
{
	if (Targets.Contains (Target))
	{
		Targets.Remove (Target);
	}
}
