// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// fixed first include
#include "ObjectFactory_Json.h"
// include STL
// includes Engine files
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameMode.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UObjectFactory_Json::UObjectFactory_Json( const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get() */ )
	: Super( ObjectInitializer )
	, SQLiteFieldsArray()
	, SQLiteFieldsMap()
{}

void UObjectFactory_Json::BeginDestroy()
{ Super::BeginDestroy(); }

void UObjectFactory_Json::ParseData( FString JsonStr )
{}

AActor* UObjectFactory_Json::SpawnActorFromDatabase( FString FieldName, FString IDToSpawn, FString BlueprintPathIdentifier /*= "BlueprintPath" */ )
{
	AActor* toReturn	= nullptr;
	UWorld* world		= nullptr;
	UObject* outer		= GetOuter();
	if ( outer->IsA( AActor::StaticClass() ) )
	{
		AActor* actor	= Cast<AActor>( outer );
		world	= actor->GetWorld();
	}
	if ( outer->IsA( AGameMode::StaticClass() ) )
	{
		AGameMode* gameMode	= Cast<AGameMode>( outer );
		world	= gameMode->GetWorld();
	}
	if ( outer->IsA( UGameInstance::StaticClass() ) )
	{
		UGameInstance* gameInstance	= Cast<UGameInstance>( outer );
		world	= gameInstance->GetWorld();
	}

	if ( ensureAlwaysMsgf( nullptr != world, *FString( "World does not exist!" ) ) )
	{
		if ( !SQLiteTableName.IsEmpty() )
		{
			if ( nullptr != SQLiteFieldsMap.Find( FieldName ) )
			{
				FSQLiteRow itemData		= GetDataByID( FieldName, IDToSpawn );
				FString* blueprintPath	= itemData.Cells.Find( BlueprintPathIdentifier );
				if ( nullptr != blueprintPath )
				{
					TSubclassOf<UObject> blueprintClass	= LoadBlueprintClass( *blueprintPath );
					if ( ensureAlwaysMsgf( nullptr != blueprintClass, *FString( "Blueprint does not exist @ " + *blueprintPath ) ) )
					{ toReturn	= world->SpawnActor<AActor>( blueprintClass, FTransform() ); }
				} //if ( nullptr != blueprintPath )
			} //if ( nullptr != SQLiteFieldsMap.Find( FieldName ) )
		} //if ( !SQLiteTableName.IsEmpty() )
	} //if ( ensureAlwaysMsgf( nullptr != world, *FString( "World does not exist!" ) ) )
	return toReturn;
}