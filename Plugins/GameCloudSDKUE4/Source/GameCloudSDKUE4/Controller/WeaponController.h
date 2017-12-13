// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <vector>
// includes Engine files
#include "Components/ActorComponent.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "WeaponController.generated.h"

class UWeaponLogic;

UENUM( BlueprintType )
enum class CanShootResult : uint8
{
	Yes,
	No,
};

USTRUCT( BlueprintType )
struct FWeaponSection
{
	GENERATED_BODY()
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<UWeaponLogic*> WeaponLogics_;
};

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UWeaponController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY( BlueprintReadWrite )
		TArray<FWeaponSection> WeaponSections_;

protected:
	TArray<int> WeaponSectionsToUpdate;
	FString OwnerName;

public:
	UWeaponController();
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller"
		, ToolTip = "Adds the weapon logic to an array of weapon logics at index Idx" ) )
		void AddWeaponLogic( UWeaponLogic* WeaponLogic, const int Idx );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller"
		, ToolTip = "Returns true if weapon is found at Index" ) )
		bool ContainsWeaponAtIndex( int Index );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller"
		, ToolTip = "Adds the weapon logic to an array of weapon logics at index Idx" ) )
		void RemoveWeaponLogic( UWeaponLogic* WeaponLogic, const int Idx );
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller", ExpandEnumAsExecs = "Result"
	//	, ToolTip = "Returns true if weapon at Idx can be shot" ) )
	//	void CanShoot( int Idx, CanShootResult& Result );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller"
		, ToolTip = "Starts shooting all weapon logics at Idx" ) )
		void ShootStart( int Idx );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Controller"
		, ToolTip = "Stops shooting all weapon logics at Idx" ) )
		void ShootEnd( int Idx );

protected:
	void BeginPlay() override;
};
