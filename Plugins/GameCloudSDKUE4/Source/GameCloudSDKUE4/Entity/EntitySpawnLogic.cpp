// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "EntitySpawnLogic.h"
// include STL
// include engine files
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// include project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Data/ObjectFactory_SQL.h"
#include "GameCloudSDKUE4/Entity/EntityPlaceholderComponent.h"

UEntitySpawnLogic::UEntitySpawnLogic()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEntitySpawnLogic::InitialiseSpawnLogic( UMeshComponent* MeshToAttachTo, TArray<USceneComponent*> PlaceholderParents )
{
	if ( ensureAlwaysMsgf( nullptr != MeshToAttachTo, *( FString( "Mesh to attach to is invalid!" ) ) ) )
	{
		// store mesh reference
		Mesh	= MeshToAttachTo;
		// store placeholders
		for ( USceneComponent* placeholderParent : PlaceholderParents )
		{
			TArray<USceneComponent*> children;
			placeholderParent->GetChildrenComponents( false, children );
			for ( USceneComponent* placeholder : children )
			{
				UEntityPlaceholderComponent* entityPlaceholder	= Cast<UEntityPlaceholderComponent>( placeholder );
				if ( nullptr != entityPlaceholder )
				{
					std::string typeString = TCHAR_TO_UTF8( *( entityPlaceholder->TypeString ) );
					std::map<std::string, std::vector<UEntityPlaceholderComponent*>>::iterator placeholderIter = Placeholders.find( typeString );
					if ( Placeholders.end() == placeholderIter )
					{
						Placeholders.insert( {typeString, std::vector<UEntityPlaceholderComponent*>()} );
						placeholderIter = Placeholders.find( typeString );
					}
					placeholderIter->second.push_back( entityPlaceholder );
				} //if ( nullptr != entityPlaceholder )
			} //for ( USceneComponent* placeholder : children )
		} //for ( USceneComponent* placeholderParent : PlaceholderParents )
		// spawn parts based on ids supplied
		//SpawnParts( IdsToSpawn );
	} //for ( USceneComponent* placeholderParent : PlaceholderParents )
}

TArray<AActor*> UEntitySpawnLogic::GetSpawnedActorsByTypeString( FString TypeString )
{
	TArray<AActor*> toReturn;
	std::string typeStringSTD( TCHAR_TO_UTF8( *TypeString ) );
	std::map<std::string, std::vector<AActor*>>::iterator iter	= SpawnedParts.find( typeStringSTD );
	if ( iter != SpawnedParts.end() )
	{
		std::vector<AActor*> actors	= iter->second;
		for ( AActor* actor : actors )
		{ toReturn.Add( actor ); }
	}
	return toReturn;
}

TArray<AActor*> UEntitySpawnLogic::GetAllSpawnParts ()
{
	TArray<AActor*> toReturn;

	std::map<std::string, std::vector<AActor*>>::iterator iter = SpawnedParts.begin ();
	for (iter; iter != SpawnedParts.end (); ++iter)
	{
		std::vector<AActor*> actors = iter->second;
		for (AActor* actor : actors)
			toReturn.Add (actor);
	}
	return toReturn;
}

void UEntitySpawnLogic::RemovePart( const FString& TypeString )
{
	std::string typeStringSTD( TCHAR_TO_UTF8( *TypeString ) );
	std::map<std::string, std::vector<AActor*>>::iterator iter	= SpawnedParts.find( typeStringSTD );
	if ( iter != SpawnedParts.end() )
	{
		std::vector<AActor*> actors	= iter->second;
		for ( AActor* actor : actors )
		{ actor->Destroy(); }

		iter->second.clear();

		std::map<std::string, FString>::iterator iter	= SpawnedIDs.find( typeStringSTD );
		if ( SpawnedIDs.end() != iter )
		{ iter->second	= FString(); }
	}
}

void UEntitySpawnLogic::ReplacePart( const FString& TypeString, const FString& IdToSpawn )
{
	RemovePart( TypeString );
	SpawnPart( TypeString, IdToSpawn );
}

void UEntitySpawnLogic::SpawnPart( const FString& TypeString, const FString& IdToSpawn )
{
	AActor* owner	= GetOwner();
	if ( ensureAlwaysMsgf( nullptr != owner, *FString( "EntitySpawnLogic does not have an owner!" ) ) )
	{
		std::string typeStdStr	= TCHAR_TO_UTF8( *( TypeString ) );
		// retrieve placeholders with the same type string
		std::map<std::string, std::vector<UEntityPlaceholderComponent*>>::iterator placeholderIter = Placeholders.find( typeStdStr );
		if ( Placeholders.end() != placeholderIter )
		{
			std::vector<UEntityPlaceholderComponent*> placeholdersOfType	= placeholderIter->second;
			for ( UEntityPlaceholderComponent* placeholder : placeholdersOfType )
			{
				// try to get factory from blueprint, if it was implemented in blueprint
				UObjectFactory_SQL* factory	= GetFactoryToSpawnFrom( placeholder );
				// factory not found, try to get from cpp implementation
				if ( nullptr == factory )
				{ factory	= GetFactoryToSpawnFrom_Implementation( placeholder ); }

				if ( ensureAlwaysMsgf( nullptr != factory, *( FString( "Factory is invalid!" ) ) ) )
				{
					FTransform transform	= placeholder->GetRelativeTransform();
					AActor* spawnedPart		= factory->SpawnActorFromDatabase( IdToSpawn, transform, owner );
					if (  nullptr != spawnedPart )
					{
						spawnedPart->SetOwner( owner );
						APawn* ownerPawn		= Cast<APawn>( owner );
						if ( nullptr != ownerPawn )
						{ spawnedPart->Instigator	= ownerPawn; }
						AttackToMesh( spawnedPart, placeholder->SocketName );

						// insert into SpawnedParts
						std::map<std::string, std::vector<AActor*>>::iterator spawnedPartsIter	= SpawnedParts.find( typeStdStr );
						if ( spawnedPartsIter == SpawnedParts.end() )
						{
							SpawnedParts.insert( {typeStdStr, std::vector<AActor*>()} );
							spawnedPartsIter	= SpawnedParts.find( typeStdStr );
						} //if ( spawnedPartsIter == SpawnedParts.end() )
						std::vector<AActor*>* container	= &spawnedPartsIter->second;
						container->push_back( spawnedPart );

						std::string typeSTDString	= TCHAR_TO_UTF8( *TypeString );
						std::map<std::string, FString>::iterator iter	= SpawnedIDs.find( typeSTDString );
						if ( SpawnedIDs.end() == iter )
						{
							SpawnedIDs.insert( {typeSTDString, ""} );
							iter	= SpawnedIDs.find( typeSTDString );
						}
						iter->second	= IdToSpawn;
					} //if ( ensureAlwaysMsgf( nullptr != spawnedPart, *( FString( "Unable to spawn part!" ) ) ) )
				} //if ( ensureAlwaysMsgf( nullptr != factory, *( FString( "Factory is not set!" ) ) ) )
			} //for ( UEntityPlaceholderComponent* placeholder : placeholdersOfType )
		} //if ( Placeholders.end() == placeholderIter )
	} //if ( ensureAlwaysMsgf( nullptr != owner, *FString( "EntitySpawnLogic does not have an owner!" ) ) )
}

void UEntitySpawnLogic::SpawnParts( const TMap<FString, FString>& IdsToSpawn )
{
	// spawn initial parts
	for ( TMap<FString, FString>::TConstIterator idIter = IdsToSpawn.CreateConstIterator(); idIter; ++idIter )
	{
		FString typeString		= idIter.Key();
		FString idString		= idIter.Value();
		SpawnPart( typeString, idString );
	} //for ( TMap<FString, FString>::TConstIterator iter = IdsToSpawn.CreateConstIterator(); iter; ++iter )
}

UObjectFactory_SQL* UEntitySpawnLogic::GetFactoryToSpawnFrom_Implementation( UEntityPlaceholderComponent* placeholder )
{
	return nullptr;
}

void UEntitySpawnLogic::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner	= GetOwner();
	if ( ensureAlwaysMsgf( nullptr != owner, *FString( "EntitySpawnLogic does not have an owner!" ) ) )
	{ OwnerName	= UKismetSystemLibrary::GetDisplayName( owner ); }
}

void UEntitySpawnLogic::AttackToMesh( AActor* ActorToAttach, FName SocketName )
{
	if ( ensureAlwaysMsgf( nullptr != Mesh, *FString( "Mesh was not set, unable to attach to mesh!" ) ) )
	{ ActorToAttach->AttachToComponent( Mesh, FAttachmentTransformRules( EAttachmentRule::KeepRelative, true ), SocketName ); }
}
