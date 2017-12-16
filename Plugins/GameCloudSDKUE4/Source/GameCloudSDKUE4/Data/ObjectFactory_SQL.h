// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <map>
#include <vector>
// includes Engine files
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Data/SQLiteDatabase.h"
#include "GameCloudSDKUE4/Data/SQLiteDatabaseStructs.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
//#include "StaticJSON/staticjson.hpp"
//#include "StaticJSON/unreal_types.h"
// fixed final include
#include "ObjectFactory_SQL.generated.h"

/**
 * 
 */

UCLASS( ClassGroup = ( DeathTrack ), Blueprintable, BlueprintType )
class GAMECLOUDSDKUE4_API UObjectFactory_SQL : public UObject
{
	GENERATED_BODY()

public:
	/**	Name of table in SQLiteDatabase this factory is related to. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, meta = ( Category = "QbjectFactory_SQL" ) )
		USQLiteDatabase* SQLiteDatabase;
	/**	Name of table in SQLiteDatabase this factory is related to. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, meta = ( Category = "QbjectFactory_SQL" ) )
		FString SQLiteTableName;
	/**	Field name that contains blueprint path used by this factory. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, meta = ( Category = "QbjectFactory_SQL" ) )
		FString BlueprintPathIdentifier;
	/**	Array of FSQLiteFields of table in SQLiteDatabase. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, meta = ( Category = "QbjectFactory_SQL" ) )
		TArray<FSQLiteField> SQLiteFieldsArray;
	/**	Map of field names and SQLiteValueTypes of table in SQLiteDatabase. */
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, meta = ( Category = "QbjectFactory_SQL" ) )
		TMap<FString, SQLiteValueTypes> SQLiteFieldsMap;

protected:

public:
	UObjectFactory_SQL( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );
	/**	To be used in place of usual destructor. */
	virtual void BeginDestroy() override;
	/*
	*	SQLString, generated through Mr. Data Converter, will be processed,
	*	used to create a table and populate the table with data from the string
	*	1) MrDataConverter will be replaced with the value of SQLiteTableName
	*	2) \r and \n will be removed
	*	3) will be split into two queries, create table and insert data
	*/
	UFUNCTION( BlueprintCallable, meta = ( Category = "QbjectFactory_SQL" ) )
		bool GenerateTableInDatabase( FString SQLString );
	UFUNCTION( BlueprintCallable, meta = ( Category = "QbjectFactory_SQL" ) )
	AActor* SpawnActorFromDatabase( FString IdToSpawn, FTransform Transform, AActor* Owner );
	/*
	*	Uses the first FSQLiteField in SQLiteFieldsArray and param RowName to
	*	retrieve from the USQLiteDatabase reference in this factory.
	*	Returns an empty FSQLiteRow if SQLiteTableName was not found in the
	*	database or the expected row was not found in the field name provided.
	*/
	UFUNCTION( BlueprintCallable, meta = ( Category = "QbjectFactory_SQL" ) )
		FSQLiteRow GetDataByID( FString ItemID );
	/*
	*	Uses param, Conditions, to retrieve from the USQLiteDatabase
	*	reference in this factory. Returns an array of FSQLiteRows that
	*	matches the search conditions provided.
	*/
	UFUNCTION( BlueprintCallable, meta = ( Category = "QbjectFactory_SQL" ) )
		TArray<FSQLiteRow> GetDataWithConditions( TArray<FString> FieldNames, TArray<FString> FieldValuesToMatch );
	/*
	*	Returns an array of FSQLiteRows that represents all data in this table.
	*/
	UFUNCTION( BlueprintCallable, meta = ( Category = "QbjectFactory_SQL" ) )
		TArray<FSQLiteRow> GetAllData();
	
	UFUNCTION( BlueprintCallable, meta = ( Category = "QbjectFactory_SQL" ) )
		bool IsFileExist( FString path );

protected:
	/**	Returns the number of Delimiter found in string ToCount. */
	int CountNumberOfDelimiterInString( FString ToCount, FString Delimiter );
	/**	Function ensures that all extra ' is escaped. */
	FString EscapeInvertedCommaForTextValue( FString Query );
	//	the following do not work in a packaged game
	//	1) StaticLoadObject()
	//	2) const IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" ).Get();
	//	Registry.GetAssetsByClass( UBlueprint::StaticClass()->GetFName(), TemplateList );
	/*
	*	A modification of EngineUtils::FindOrLoadAssetsByPath().

	*	StaticLoadObject() and 

	const IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( "AssetRegistry" ).Get();
	Registry.GetAssetsByClass( UBlueprint::StaticClass()->GetFName(), TemplateList );
	*/
	UClass* GetBlueprintClass( FString BlueprintPath );
	/*
	*	Using the query to create table, SQLiteFieldsArray and SQLiteFieldsMap
	*	are populated
	*/
	void PopulateSQLiteFields( FString CreateTableQuery );
	/*
	*	Function runs a while loop to remove all ToRemove in RemoveFrom
	*/
	FString RemoveFromString( FString RemoveFrom, FString ToRemove );
	/*
	*	Removes whitespace(s) from RemoveFrom found around Delimiter
	*/
	FString RemoveWhiteSpaceAroundDelimiter( FString RemoveFrom, FString Delimiter );
	/*
	*	Separates ToSplit into an individual string when Delimiter is found.
	*	KeepDelimiter will leave delimiter with the separated front string if true.
	*/
	TArray<FString> SplitStringWithDelimiter( FString ToSplit, FString Delimiter, const bool KeepDelimiter );

	TSubclassOf<UObject> TestFunc( FName Name, const TCHAR* Path );
};