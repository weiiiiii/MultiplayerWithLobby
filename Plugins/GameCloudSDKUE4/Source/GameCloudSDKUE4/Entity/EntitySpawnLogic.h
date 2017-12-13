// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <map>
#include <vector>
// includes Engine files
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Map.h"
// includes plugin files
// includes third party files
// includes project files
// fixed final include
#include "EntitySpawnLogic.generated.h"

class UMeshComponent;
class UObjectFactory_SQL;
class UEntityPlaceholderComponent;

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), Blueprintable, meta = ( BlueprintSpawnableComponent ) )
class GAMECLOUDSDKUE4_API UEntitySpawnLogic : public UActorComponent
{
	GENERATED_BODY()

public:
	/**	Mesh where spawned actors will be attached to. */
	UPROPERTY( BlueprintReadWrite, meta = ( Category = "Entity" ) )
		UMeshComponent* Mesh;
	/**	Map of EntityPlaceholderComponents with their type as key. */
		std::map<std::string, std::vector<UEntityPlaceholderComponent*>> Placeholders;
	/**	Actors spawned and attached to mesh, using TypeString of the EntityPlaceholderComponent as key. */
		std::map<std::string, std::vector<AActor*>> SpawnedParts;
	/**	ID strings of Actors spawned and attached to mesh, using TypeString of the EntityPlaceholderComponent as key. */
		std::map<std::string, FString> SpawnedIDs;

protected:
	FString OwnerName;


public:
	UEntitySpawnLogic();
	/**	Initialise variables in Entity Spawn Logic. */
	UFUNCTION( BlueprintCallable, meta = ( Category = "Entity", Tooltip = "" ) )
		void InitialiseSpawnLogic( UMeshComponent* MeshToAttachTo, TArray<USceneComponent*> PlaceholderParents );
	/**	Returns spawned actors that matches the string. */
	UFUNCTION( BlueprintCallable, Category = "Entity" )
		TArray<AActor*> GetSpawnedActorsByTypeString( FString TypeString );
	UFUNCTION (BlueprintCallable, Category = "DeathTrack|Car Logic|SpawnLogic")
		TArray<AActor*> GetAllSpawnParts ();
	/**	Removes all spawned parts that matches TypeString. */
	UFUNCTION( BlueprintCallable, BlueprintCallable, Category = "Entity" )
		void RemovePart( const FString& TypeString );
	/**	Removes all spawned parts that matches TypeString and replaces them with parts that matches IdToSpawn */
	UFUNCTION( BlueprintCallable, BlueprintCallable, Category = "Entity" )
		void ReplacePart( const FString& TypeString, const FString& IdToSpawn );
	/**	Spawn part listed in IdToSpawn. */
	UFUNCTION( BlueprintCallable, BlueprintCallable, Category = "Entity" )
		void SpawnPart( const FString& TypeString, const FString& IdToSpawn );
	/**	Spawn parts listed in IdsToSpawn. */
	UFUNCTION( BlueprintCallable, BlueprintCallable, Category = "Entity" )
		void SpawnParts( const TMap<FString, FString>& IdsToSpawn );

	//// may be removed, since SpawnedID is exposed to blueprint
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "" ) )
	//	TMap<FString, FString> GetSpawnedIDs();
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "" ) )
	//	TArray<AActor*> GetPartsByType( FString type );
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "Spawns temporary part(s) using the type and id, hides existing part(s) of the same type" ) )
	//	void PreviewPart( FString type, FString id );
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "Removes existing part(s) that matches type" ) )
	//	void RemovePart( FString type );
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity" ) )
	//	void SetupSpawnLogic( UStaticMeshComponent* MeshComponent, TArray<USceneComponent*> placeholderParents );
	//UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "Searches available data factories in game instance for data of part to spawn, MUST BE OVERRIDDEN" ) )
	//	void SpawnPart( const FString& type, const FString& id );
	////void SpawnPart_Implementation( FString type, FString id );
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "" ) )
	//	void SpawnParts();
	//UFUNCTION( BlueprintCallable, meta = ( Category = "GameCloudSDKUE4|Entity", Tooltip = "" ) )
	//	void SwapPart( FString type );

	/**	Searches available data factories in game instance for data of part to spawn. */
	UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category = "Entity" )
		UObjectFactory_SQL* GetFactoryToSpawnFrom( const UEntityPlaceholderComponent* placeholder );
	virtual UObjectFactory_SQL* GetFactoryToSpawnFrom_Implementation( UEntityPlaceholderComponent* placeholder );

protected:
	virtual void BeginPlay() override;
	void AttackToMesh( AActor* ActorToAttach, FName SocketName );
};
