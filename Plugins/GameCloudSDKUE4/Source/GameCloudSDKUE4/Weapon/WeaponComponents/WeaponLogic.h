// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Weapon/BaseProjectile_CPP.h"
// fixed final include
#include "WeaponLogic.generated.h"

class USceneComponent;
class UWeaponType_Range;

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnShootStart );
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnShootEnd );

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UWeaponLogic : public UActorComponent
{
	GENERATED_BODY()

	enum WeaponState
	{
		IDLING,
		COOLING_DOWN,
		RELOADING,
		CHARGING,
	};

public:
	/**	Delegate will broadcast on ShootStart(). */
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		FOnShootStart OnShootStart;
	/**	Delegate will broadcast on ShootStart(). */
	UPROPERTY( BlueprintAssignable, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		FOnShootEnd OnShootEnd;
	/**	Projectile to spawn. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		TSubclassOf<AActor> ProjectileType_;
	/**	The mechanics to use for this weapon. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		TSubclassOf<UWeaponType_Range> RangeWeaponType_;
	/**	Reference of Range Weapon Type created on event BeginPlay. */
	UPROPERTY( BlueprintReadOnly, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		UWeaponType_Range* RangeWeaponTypeReference_;

	/*
	*	Maximum ammunition count
	*	Set this to 0 to indicate that this weapon does not require reload
	*/
	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		int32 MaxAmmoCount_;
	/**	Time to wait before weapon can shoot again. */
	UPROPERTY ( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		float Cooldown_;
	/**	If true, weapon can shoot continuously while holding down shoot button. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		bool ShootOnHold_;
	/**	The random roll, pitch and yaw when a projectile is fired. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Weapon|WeaponComponents" ) )
		FRotator WeaponSpread;
	/**	Range where projectile can deal damage. */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "GameCloudSDKUE4|Weapon|WeaponComponents" )
		float EffectiveRange;
	/**	Channel the beam has to trace in. */
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, Category = "GameCloudSDKUE4|Weapon|WeaponComponents" )
		TEnumAsByte<ECollisionChannel> CollisionChannel;

protected:
	// amount of ammunition left in magazine 
	int CurrentAmmoCount_;
	float CooldownTimer_;
	WeaponState WeaponState_;
	FString OwnerName_;
	bool IsShooting;

public:
	// Sets default values for this component's properties
	UWeaponLogic();
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;	
	/**	Function will call Shoot() and OnShootStart will broadcast if IsShooting is false*/
	void ShootStart( FString& ErrorString );
	/**	Function will call Shoot() */
	void ShootUpdate( FString& ErrorString );
	/**	Function will trigger RangeWeaponTypeReference_'s ShootEnd() and OnShootEnd will broadcast */
	void ShootEnd( FString& ErrorString );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/**	Creates RangeWeaponTypeReference_ using RangeWeaponType_*/
	void CreateRangeWeapon();
	/**	Checks validity of RangeWeaponTypeReference_, CheckIsCooldowned(), CheckHasAmmo() and if weapon can fire two or more shots*/
	bool CheckCanShoot( FString& ErrorString );
	/**	Checks if 0 >= CurrentAmmoCount*/
	bool CheckHasAmmo();
	/**	Checks if 0 >= Cooldown_ */
	bool CheckIsCooldowned();
};
