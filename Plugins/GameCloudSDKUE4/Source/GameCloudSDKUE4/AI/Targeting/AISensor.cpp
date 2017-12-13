// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "AISensor.h"
// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Entity/EntityComponent.h"

UAISensor::UAISensor()
	: Range_( 0.0f )
{

}

bool UAISensor::IsTargetInRange( AActor* target )
{
	AActor* owner = GetOwner();
	check( owner );

	FVector targetLocation = target->GetActorLocation();
	FVector componentLocation = owner->GetActorLocation();
	float distance = FVector::Dist( targetLocation, componentLocation );
	return distance < Range_;
}

void UAISensor::BeginPlay()
{
	Super::BeginPlay();
}

void UAISensor::FilterTargets( TArray<AActor*>& targets )
{
	TArray<AActor*> toReturn;
	for ( AActor* target : targets )
	{
		if ( IsValidTarget( target ) )
		{ toReturn.Push( target ); }
	}

	targets = toReturn;
}

TArray<AActor*> UAISensor::GetTargets()
{
	AActor* owner = GetOwner();
	check( owner );
	UWorld* world = owner->GetWorld();
	check( world );

	TArray<class AActor*> actorsInRange;
	UKismetSystemLibrary::SphereOverlapActors( world, owner->GetActorLocation(), Range_, TArray<TEnumAsByte<EObjectTypeQuery>>(), nullptr, TArray<AActor*>(), actorsInRange );
	FilterTargets( actorsInRange );
	return actorsInRange;
}
