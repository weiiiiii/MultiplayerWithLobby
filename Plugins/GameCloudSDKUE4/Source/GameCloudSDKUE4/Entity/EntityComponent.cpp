// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "EntityComponent.h"
// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UEntityComponent::UEntityComponent()
	: OnEntityKill()
	, OnEntityDestroy()
	, EntityData()
	//: EntityData()
	, AutoDestroyOnEntityKill( false )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEntityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEntityComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner = GetOwner();
	check( owner );
	ensureMsgf( nullptr == owner->GetParentActor(), *FString( "EntityComponent MUST be added to the base actor!" ) );

	//owner->OnTakePointDamage.AddDynamic( this, &UEntityComponent::OnTakePointDamage );
	owner->OnTakeAnyDamage.AddDynamic( this, &UEntityComponent::OnTakeAnyDamage );
	owner->OnDestroyed.AddDynamic( this, &UEntityComponent::OnDestroyed );
}

void UEntityComponent::EntityTakeDamage( float Damage, const class UDamageType* DamageType )
{
	EntityData.Health -= Damage;
	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "UEntityComponent::EntityTakeDamage - Health left : " + FString::SanitizeFloat( EntityData.Health ) );
	OnEntityDamaged.Broadcast( Damage );

	AActor* owner = GetOwner();
	check( owner );
	DebugDamageTaken.PrintMessageOnEvent( UKismetSystemLibrary::GetDisplayName( owner ) + " took " + FString::SanitizeFloat( Damage ) + " damage!" );
}

void UEntityComponent::OnTakeAnyDamage( AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser )
{
	AActor* owner = GetOwner();
	check( owner );
	FString ownerName = UKismetSystemLibrary::GetDisplayName( owner );
	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Green, InstigatedBy->name );
	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Cyan, "UEntityComponent::OnTakeAnyDamage - " + ownerName );

	EntityTakeDamage( Damage, DamageType );
	if ( 0.0f >= EntityData.Health )
	{
		EntityData.Health = 0.0f;
		EntityData.IsDead = true;

		OnEntityKill.Broadcast( owner, InstigatedBy );
		if ( AutoDestroyOnEntityKill )
		{ owner->Destroy(); }
	}
}

void UEntityComponent::OnTakePointDamage( AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser )
{
	AActor* owner = GetOwner();
	check( owner );
	FString ownerName = UKismetSystemLibrary::GetDisplayName( owner );
	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Green, ownerName + " is taking " + FString::SanitizeFloat( Damage ) + " damage!" );
	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Magenta, "UEntityComponent::OnTakePointDamage - " + ownerName );

	EntityTakeDamage( Damage, DamageType );
	if ( 0.0f >= EntityData.Health )
	{
		EntityData.Health = 0.0f;
		EntityData.IsDead = true;

		OnEntityKill.Broadcast( owner, InstigatedBy );
		if ( AutoDestroyOnEntityKill )
		{ owner->Destroy(); }
	}
}

void UEntityComponent::OnDestroyed( AActor* DestroyedActor )
{
	OnEntityDestroy.Broadcast( DestroyedActor );
}
