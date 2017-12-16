// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WeaponController.h"
// include STL
// includes Engine files
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Weapon/WeaponComponents/WeaponLogic.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UWeaponController::UWeaponController()
	: WeaponSections_()
	, WeaponSectionsToUpdate()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponController::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	for ( int index : WeaponSectionsToUpdate )
	{
		if ( ensureAlwaysMsgf( WeaponSections_.Num() > index, *( FString( OwnerName + " attempting to update weapons at index " + FString::FromInt( index ) + ", index is out of bound!" ) ) ) )
		{
			FWeaponSection section = WeaponSections_[index];
			for ( auto weaponLogic : section.WeaponLogics_ )
			{
				FString errorStr = FString();
				if ( ensureAlwaysMsgf( nullptr != weaponLogic, *( FString( OwnerName + " contains an invalid weapon logic." ) ) ) )
				{ weaponLogic->ShootUpdate( errorStr ); }
			} //for ( auto weaponLogic : section.WeaponLogics_ )
		} //if ( ensureAlwaysMsgf( WeaponSections_.Num() > index, *( FString( OwnerName + " attempting to shoot weapons from index " + FString::FromInt( index ) + ", index is out of bound!" ) ) ) )
	} //for ( int index : WeaponSectionsToUpdate )
}

void UWeaponController::AddWeaponLogic( UWeaponLogic* WeaponLogic, const int Idx )
{
	if ( ensureAlwaysMsgf( nullptr != WeaponLogic, *FString( OwnerName + " is attempting to add invalid WeaponLogic!" ) ) )
	{
		if ( Idx >= WeaponSections_.Num() )
		{
			for ( int i = WeaponSections_.Num(); i < Idx + 1; i++ )
				WeaponSections_.Add( FWeaponSection() );
		} //if ( Idx >= WeaponSections_.Num() )
		WeaponSections_[Idx].WeaponLogics_.Push( WeaponLogic );
	} //if ( ensureAlwaysMsgf( nullptr != WeaponLogic, *FString( OwnerName + " is attempting to add invalid WeaponLogic!" ) ) )
}

bool UWeaponController::ContainsWeaponAtIndex( int Index )
{
	bool result	= false;
	if ( WeaponSections_.Num() > Index )
	{
		FWeaponSection section = WeaponSections_[Index];
		for ( auto weaponLogic : section.WeaponLogics_ )
		{
			if ( ensureAlwaysMsgf( nullptr != weaponLogic, *( FString( OwnerName + " contains invalid weapon logic" ) ) ) )
			{ result	= true; }
		}
	} //if ( WeaponSections_.Num() > Index )
	return result;
}

void UWeaponController::RemoveWeaponLogic( UWeaponLogic* WeaponLogic, const int Idx )
{
	if ( ensureAlwaysMsgf( nullptr != WeaponLogic, *FString( OwnerName + " is attempting to remove invalid WeaponLogic!" ) ) )
	{
		if ( ensureAlwaysMsgf( WeaponSections_.Num() > Idx, *( FString( OwnerName + " attempting to remove weapon logic at index " + FString::FromInt( Idx ) + ", index is out of bound!" ) ) ) )
		{
			int weaponLogicIndex	= 0;
			if ( WeaponSections_[Idx].WeaponLogics_.Find( WeaponLogic, weaponLogicIndex ) )
			{ WeaponSections_[Idx].WeaponLogics_.Remove( WeaponLogic ); }
			else
			{ UCommonFunctions::PrintToLog( OwnerName + " does not have specified weapon logic not found, unable to remove!" ); }
		} //if ( ensureAlwaysMsgf( WeaponSections_.Num() > index, *( FString( OwnerName + " attempting to remove weapon logic from index " + FString::FromInt( index ) + ", index is out of bound!" ) ) ) )
	} //if ( ensureAlwaysMsgf( nullptr != WeaponLogic, *FString( OwnerName + " is attempting to remove invalid WeaponLogic!" ) ) )
}

//void UWeaponController::CanShoot( int Idx, CanShootResult& Result )
//{
//	if ( WeaponSections_.Num() > Idx )
//	{
//		FWeaponSection weaponSection = WeaponSections_[Idx];
//
//		Result = ( weaponSection.WeaponLogics_.Num() > 0 ? CanShootResult::Yes : CanShootResult::No );
//	}
//	else
//	{ Result = CanShootResult::No; }
//}

void UWeaponController::ShootStart( int Idx )
{
	if ( ensureAlwaysMsgf( WeaponSections_.Num() > Idx, *( FString( OwnerName + " attempting to start shooting weapons at index " + FString::FromInt( Idx ) + ", index is out of bound!" ) ) ) )
	{
		FWeaponSection section = WeaponSections_[Idx];
		for ( auto weaponLogic : section.WeaponLogics_ )
		{
			if ( ensureAlwaysMsgf( nullptr != weaponLogic, *( FString( OwnerName + " attempting to start shooting an invalid weapon logic" ) ) ) )
			{
				FString errorStr = FString();
				weaponLogic->ShootStart( errorStr );
			} //if ( ensureAlwaysMsgf( nullptr != weaponLogic, *( FString( OwnerName + " attempting to start shooting an invalid weapon logic" ) ) ) )
		} //for ( auto weaponLogic : section.WeaponLogics_ )
		WeaponSectionsToUpdate.Add( Idx );
	} //if ( ensureAlwaysMsgf( WeaponSections_.Num() > Idx, *( FString( OwnerName + " attempting to remove weapon logic from index " + FString::FromInt( index ) + ", index is out of bound!" ) ) ) )
}

void UWeaponController::ShootEnd( int Idx )
{
	//if ( ensureAlwaysMsgf( WeaponSections_.Num() > Idx, *( FString( OwnerName + " attempting to stop shooting weapons at index " + FString::FromInt( Idx ) + ", index is out of bound!" ) ) ) )
	if ( WeaponSections_.Num() > Idx )
	{
		FWeaponSection section = WeaponSections_[Idx];
		for ( auto weaponLogic : section.WeaponLogics_ )
		{
			//if ( ensureAlwaysMsgf( nullptr != weaponLogic, *( FString( OwnerName + " attempting to stop shooting an invalid weapon logic" ) ) ) )
			if ( nullptr != weaponLogic )
			{
				FString errorStr = FString();
				weaponLogic->ShootEnd( errorStr );
			} //if ( ensureAlwaysMsgf( nullptr != weaponLogic, *( FString( OwnerName + " attempting to stop shooting an invalid weapon logic" ) ) ) )
		} //for ( auto weaponLogic : section.WeaponLogics_ )
		WeaponSectionsToUpdate.Remove( Idx );
	} //if ( ensureAlwaysMsgf( WeaponSections_.Num() > Idx, *( FString( OwnerName + " attempting to stop shooting weapons at index " + FString::FromInt( Idx ) + ", index is out of bound!" ) ) ) )
}

void UWeaponController::BeginPlay()
{
	AActor* owner	= GetOwner();
	OwnerName		= UKismetSystemLibrary::GetDisplayName( owner );
}
