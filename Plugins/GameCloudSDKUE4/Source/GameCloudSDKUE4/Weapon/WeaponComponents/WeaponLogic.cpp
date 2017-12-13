// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WeaponLogic.h"
// include STL
// includes Engine files
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "WeaponType_Range.h"

UWeaponLogic::UWeaponLogic()
	: RangeWeaponTypeReference_( nullptr )
	, CurrentAmmoCount_( 0 )
	, CooldownTimer_ ( 0.0f )
	, ShootOnHold_ ( true )
	, WeaponState_( IDLING )
{
	PrimaryComponentTick.bCanEverTick = true;	
}

void UWeaponLogic::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// try to cool down weapon
	if ( 0 < CooldownTimer_ )
	{ CooldownTimer_ -= DeltaTime; }
	else
	{
		// disable tick if weapon is no longer shooting
		if ( !IsShooting )
		{ PrimaryComponentTick.SetTickFunctionEnable( false ); }
	}
}

bool UWeaponLogic::CheckCanShoot( FString& ErrorString )
{
	bool result	= false;
	if ( nullptr != RangeWeaponTypeReference_ )
	{
		if ( CheckHasAmmo() )
		{
			if ( CheckIsCooldowned() )
			{
				if ( !ShootOnHold_ )
				{
					if ( RangeWeaponTypeReference_->GetHasShot() )
					{ ErrorString	= OwnerName_ + " does not allow continuous shooting"; }
					else
					{ result	= true; }
				} //if ( !ShootOnHold_ )
				else
				{ result	= true; }
			} //if ( CheckIsCooldowned() )
			else
			{ ErrorString	= OwnerName_ + " is still cooling down"; }
		} //if ( CheckHasAmmo() )
		else
		{ ErrorString	= OwnerName_ + " has no ammunition left"; }
	} //if ( nullptr != RangeWeaponTypeReference_ )
	else
	{ ErrorString	= OwnerName_ + " does not contain a valid RangeWeaponType reference"; }
	return result;	
}

void UWeaponLogic::ShootStart( FString& ErrorString )
{
	if ( CheckCanShoot( ErrorString ) )
	{
		RangeWeaponTypeReference_->ShootStart();
		CooldownTimer_ = Cooldown_;

		if ( !IsShooting )
		{
			OnShootStart.Broadcast();
			IsShooting	= true;
			PrimaryComponentTick.SetTickFunctionEnable( true );
		}
	}
}

void UWeaponLogic::ShootUpdate( FString& ErrorString )
{ 
	if ( CheckCanShoot( ErrorString ) )
	{
		RangeWeaponTypeReference_->ShootUpdate();
		CooldownTimer_ = Cooldown_;
	}
}

void UWeaponLogic::ShootEnd( FString& ErrorString )
{
	if ( IsShooting )
	{
		OnShootEnd.Broadcast();
		IsShooting	= false;
		RangeWeaponTypeReference_->ShootEnd();
	} //if ( IsShooting )
}

void UWeaponLogic::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmoCount_ =  MaxAmmoCount_;
	CreateRangeWeapon();
}

void UWeaponLogic::CreateRangeWeapon()
{
	auto owner = GetOwner();
	check( nullptr != owner );
	// set owner name, to facilitate error logging
	OwnerName_ = UKismetSystemLibrary::GetDisplayName( GetOwner() );

	checkf( nullptr != RangeWeaponType_, *FString( OwnerName_ + " already has a RangeWeaponType created!" ) );

	RangeWeaponTypeReference_ = NewObject<UWeaponType_Range> ( owner, RangeWeaponType_ );
	RangeWeaponTypeReference_->RegisterComponent ();
	RangeWeaponTypeReference_->ProjectileType	= ProjectileType_;
	RangeWeaponTypeReference_->WeaponSpread		= this->WeaponSpread;
	RangeWeaponTypeReference_->EffectiveRange	= this->EffectiveRange;
	RangeWeaponTypeReference_->CollisionChannel	= this->CollisionChannel;
}

bool UWeaponLogic::CheckHasAmmo()
{
	if ( 0 >= MaxAmmoCount_ )
		return true;
	return 0 < CurrentAmmoCount_;
}

bool UWeaponLogic::CheckIsCooldowned()
{ return 0 >= CooldownTimer_; }
