// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <unordered_map>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Containers/Map.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "VehicleSpawnLogic.generated.h"

class UVehiclePart;

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UVehicleSpawnLogic : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnyWhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		UDataTable* VehicleData;

protected:
	enum PartStatus
	{
		STANDBY_LEFT,
		SPAWNED,
		STANDBY_RIGHT,
		VIEWING
	};

	std::unordered_map<VehiclePartType, std::vector<UVehiclePart*>> placeholders;
	std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>> spawnableClasses;
	std::unordered_map<VehiclePartType, TSubclassOf<AActor>> defaultSpawnableClasses;
	std::unordered_map<VehiclePartType, int> defaultSpawnableIndices;
	std::unordered_map<VehiclePartType, std::vector<AActor*>> spawnedParts;
	std::unordered_map<VehiclePartType, int> spawnedIndices;
	std::unordered_map<VehiclePartType, std::vector<AActor*>> viewingParts;
	std::unordered_map<VehiclePartType, int> viewingIndices;

	std::unordered_map<VehiclePartType, std::vector<AActor*>> standbyPartsLeft;
	std::unordered_map<VehiclePartType, int> standbyIndicesLeft;
	std::unordered_map<VehiclePartType, std::vector<AActor*>> standbyPartsRight;
	std::unordered_map<VehiclePartType, int> standbyIndicesRight;

	bool isFromMenu;

public:
	UVehicleSpawnLogic();
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		TMap<VehiclePartType, int> GetSpawnedIndices();
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		TArray<AActor*> GetPartsByType( VehiclePartType type );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		void PreviewPart( VehiclePartType type, bool isViewingPrev );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		void ResetToDefault( VehiclePartType type );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		void SelectPreviewPart( VehiclePartType type );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		void SetupPlaceholders( USceneComponent* placeholdersParent );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		// error C2275 illegal use of this type as an expression
		// https://issues.unrealengine.com/issue/UE-42521 to be fixed in 4.17!
		//void SpawnParts( TMap<VehiclePartType,int> PartIndices, bool DisableCollision );
		void SpawnParts( TArray<int> PartIndices, bool FromMenu );
	UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Vehicle" ) )
		void SwapPart( VehiclePartType type, bool isPrev );

protected:
	virtual void BeginPlay() override;

  	void loadData();
	void hideActors( std::vector<AActor*> actors, bool isHiding );
	void hideSpawnedParts( VehiclePartType type, bool isHiding );
	void removePart( VehiclePartType type, PartStatus status, bool destroyExisting );
	void replaceIndex( VehiclePartType type, PartStatus status, int newIndex );
	void replacePart( VehiclePartType type, PartStatus status, std::vector<AActor*> actors, bool destroyExisting );
	void spawnPart( VehiclePartType type, PartStatus status, bool destroyExisting );
	void warpValueInRange( int &value, int min, int max );
};
