// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "AISensorBase.h"
// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Engine/World.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Entity/EntityComponent.h"

UAISensorBase::UAISensorBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UAISensorBase::IsTargetInRange( AActor* target )
{
	checkf( false, *FString( "IsTargetInRange() must be overridden!" ) );
	return false;
}

TArray<AActor*> UAISensorBase::GetTargets()
{
	checkf( false, *FString( "GetTargetsInRange() must be overridden!" ) );
	return TArray<AActor*>();
}

void UAISensorBase::BeginPlay()
{
	Super::BeginPlay();
}

bool UAISensorBase::IsValidTarget( AActor* target )
{
	bool result = false;

	UEntityComponent* entityComponent = Cast<UEntityComponent>( target->GetComponentByClass( UEntityComponent::StaticClass() ) );
	if ( nullptr != entityComponent )
	{
		AActor* owner = this->GetOwner();
		check( owner );

		AActor* baseParent = GetBaseActor( owner );
		if ( baseParent != target )
		{ result = true; }
	}
	return result;
}

AActor* UAISensorBase::GetBaseActor( AActor* target )
{
	AActor* toReturn	= target;
	AActor* parent		= target->GetOwner();
	// target does not have owner, try to get parent through GetParentActor()
	if ( nullptr == parent )
	{ parent = target->GetParentActor(); }
	// parent found, recurse function
	if ( nullptr != parent )
	{ toReturn = GetBaseActor( parent ); }

	return toReturn;
}

//TArray<AActor*> UAISensorBase::GetTargetsByDistance( int Count, bool Closest )
//{
//	TArray<AActor*> targets = GetTargetsInRange();
//	TArray<AActor*> toReturn;
//
//	//if ( 1 == targets.Num() )
//	//{
//	//	toReturn = targets;
//	//}
//	//else if ( 2 <= targets.Num() )
//	//{
//	// sorts targets based on distance from Owner_
//	//https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/TArrays/
//	targets.StableSort( [this, Closest]( AActor& A, AActor& B )
//	{
//		float distA = FVector::Distance( ( A ).GetActorLocation(), Owner_->GetActorLocation() );
//		float distB = FVector::Distance( ( B ).GetActorLocation(), Owner_->GetActorLocation() );
//		if ( Closest )
//		{
//			return distA < distB;
//		}
//		else
//		{
//			return distA > distB;
//		}
//	} );
//	//}
//
//	return targets;
//}