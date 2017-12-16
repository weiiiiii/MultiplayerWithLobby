// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// fixed first include
#include "ObjectFactory_SQL.h"
// include STL
// includes Engine files
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Paths.h"
#include "GameFramework/GameMode.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UObjectFactory_SQL::UObjectFactory_SQL( const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get() */ )
	: Super( ObjectInitializer )
	, SQLiteTableName()
	, BlueprintPathIdentifier( "Path" )
	, SQLiteFieldsArray()
	, SQLiteFieldsMap()
{}

void UObjectFactory_SQL::BeginDestroy()
{ Super::BeginDestroy(); }

bool UObjectFactory_SQL::GenerateTableInDatabase( FString SQLString )
{
	bool result	= false;
	if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "Database was not set!" ) ) )
	{
		if ( nullptr != SQLiteDatabase )
		{
			if ( !SQLiteTableName.IsEmpty() )
			{
				// "MrDataConverter" will be replaced with the value of SQLiteTableName
				SQLString	= SQLString.Replace( *FString( "MrDataConverter" ), *SQLiteTableName, ESearchCase::IgnoreCase );
				// this field will be removed, causing error in SQLite3
				SQLString	= RemoveFromString( SQLString, "id INT NOT NULL AUTO_INCREMENT PRIMARY KEY," );
				// remove "\r"
				SQLString	= RemoveFromString( SQLString, "\r" );
				// remove "\n"
				SQLString	= RemoveFromString( SQLString, "\n" );
				// split SQLString into create table and insert into queries
				TArray<FString> queries	= SplitStringWithDelimiter( SQLString, ";", true );

				// there should only be "CREATE TABLE" and "INSERT INTO" queries
				if ( ensureAlwaysMsgf ( 2 == queries.Num(), *FString( SQLiteTableName + " is using invalid SQL data!" ) ) )
				{
					// CREATE TABLE query will be the first query
					PopulateSQLiteFields( queries[0] );
					queries[1]	= EscapeInvertedCommaForTextValue( queries[1] );
					result	= true;
					// execute queries
					for ( int i = 0; i < queries.Num(); i++ )
					{
						FString query	= queries[i];
						if ( !SQLiteDatabase->ExecSql( query ) )
						{ result	= false; }
					} //for ( int i = 0; i < queries.Num(); i++ )
				} //if ( 2 == queries.Num() )
			} //if ( !SQLiteTableName.IsEmpty() )
		} //if ( nullptr != gameInstance->Database )
	} //if ( ensureAlwaysMsgf( nullptr != Database, *FString( "Database was not set!" ) ) )
	return result;
}

AActor* UObjectFactory_SQL::SpawnActorFromDatabase( FString IdToSpawn, FTransform Transform, AActor* Owner )
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
			if ( 0 < SQLiteFieldsArray.Num() )
			{
				FSQLiteRow itemData		= GetDataByID( IdToSpawn );
				FString* blueprintPath	= itemData.Cells.Find( BlueprintPathIdentifier );
				if ( nullptr != blueprintPath )
				{
					FActorSpawnParameters params;
					params.Owner	= Owner;
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					APawn* pawn		= Cast<APawn>( Owner );
					if ( nullptr != pawn )
					{ params.Instigator	= pawn; }

					UClass* blueprintClass	= GetBlueprintClass( *blueprintPath ); 
					if ( ensureAlwaysMsgf( nullptr != blueprintClass, *FString( "Blueprint does not exist @ " + *blueprintPath ) ) )
					{
						toReturn		= world->SpawnActor<AActor>( blueprintClass, Transform, params );
						if (nullptr != toReturn)
						{
							FVector scale = Transform.GetScale3D ();
							toReturn->SetActorRelativeScale3D (scale);
						}
						else
						{
							UCommonFunctions::PrintFStringOnScreen (5.0f, FColor::Red, "Unable to spawn " + IdToSpawn);
							UCommonFunctions::PrintToLog ("Unable to spawn " + IdToSpawn);
						}
					} //if ( ensureAlwaysMsgf( nullptr != blueprintClass, *FString( "Blueprint does not exist @ " + *blueprintPath ) ) )

					//TSubclassOf<UObject> theClass	= TestFunc( FName( "Name" ), *(*blueprintPath ) );
					//if ( nullptr != theClass )
					//{
					//	world->SpawnActor<AActor>( theClass, Transform, params );
					//	UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Green, "New way to get class SUCCEEDED!" );
					//}
					//else
					//{ UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "New way to get class FAILED!" ); }

				} //if ( nullptr != blueprintPath )
			} //if ( 0 < SQLiteFieldsArray.Num() )
		} //if ( !SQLiteTableName.IsEmpty() )
	} //if ( ensureAlwaysMsgf( nullptr != world, *FString( "World does not exist!" ) ) )
	return toReturn;
}

FSQLiteRow UObjectFactory_SQL::GetDataByID( FString RowName )
{
	FSQLiteRow result;
	if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "Database was not set!" ) ) )
	{
		if ( 0 < SQLiteFieldsArray.Num() )
		{
			// first field should be the row identifier
			FSQLiteField rowIdentifierField	= SQLiteFieldsArray[0];

			FString columnName			= rowIdentifierField.Name;
			SQLiteValueTypes valueType	= rowIdentifierField.Type;
			if ( SQLiteValueTypes::TEXT == valueType )
			{ result	= SQLiteDatabase->GetRow( SQLiteTableName, columnName, "'" + RowName + "'" ); }
			else
			{ result	= SQLiteDatabase->GetRow( SQLiteTableName, columnName, RowName ); }
		} //if ( 0 < SQLiteFieldsArray.Num() )
	} //if ( ensureAlwaysMsgf( nullptr != Database, *FString( "Database was not set!" ) ) )
	return result;
}

TArray<FSQLiteRow> UObjectFactory_SQL::GetDataWithConditions( TArray<FString> FieldNames, TArray<FString> FieldValuesToMatch )
{
	TArray<FSQLiteRow> result;
	if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "Database was not set!" ) ) )
	{
		for ( int i = 0; i < FieldNames.Num(); i++ )
		{
			FString fieldName		= FieldNames[i];
			SQLiteValueTypes* type	= SQLiteFieldsMap.Find( fieldName );
			if ( nullptr != type )
			{
				if ( SQLiteValueTypes::TEXT == *type )
				{
					//FString newFieldName	= "'" + fieldName + "'";
					//FieldNames[i]			= newFieldName;

					FString fieldValue		= FieldValuesToMatch[i];
					FString newFieldValue	= "'" + fieldValue + "'";
					FieldValuesToMatch[i]	= newFieldValue;
				} //if ( SQLiteValueTypes::TEXT == *type )
			} //if ( nullptr != type )
		} //for ( int i = 0; i < FieldNames.Num(); i++ )

		result	= SQLiteDatabase->GetRows( SQLiteTableName, FieldNames, FieldValuesToMatch );
	} //if ( ensureAlwaysMsgf( nullptr != Database, *FString( "Database was not set!" ) ) )
	return result;
}

TArray<FSQLiteRow> UObjectFactory_SQL::GetAllData()
{
	TArray<FSQLiteRow> result;
	if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "Database was not set!" ) ) )
	{ result	= SQLiteDatabase->GetTable( SQLiteTableName ); }
	return result;
}

bool UObjectFactory_SQL::IsFileExist( FString path )
{
	return FPaths::FileExists( path );
}

int UObjectFactory_SQL::CountNumberOfDelimiterInString( FString ToCount, FString Delimiter )
{
	int toReturn	= 0;
	int index	= ToCount.Find( Delimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	while ( -1 != index )
	{
		toReturn++;
		ToCount.RemoveAt( 0, index + Delimiter.Len() );
		index	= ToCount.Find( Delimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	}

	return toReturn;
}

FString UObjectFactory_SQL::EscapeInvertedCommaForTextValue( FString Query )
{
	FString toReturn;

	FString valueKey	= "VALUES";
	int index	= Query.Find( valueKey, ESearchCase::CaseSensitive, ESearchDir::FromStart );
	if ( -1 != index )
	{
		// Find out why this part removes too much data, all the way up to BMW5...
		toReturn	+= Query.Left( index + valueKey.Len() );
		Query.RemoveAt( 0, index + valueKey.Len() );
		// Remove all whitespace from INSERT INTO query, which is always the second query
		Query	= RemoveFromString( Query, " " );

		TArray<FString> rows	= SplitStringWithDelimiter( Query, ")", true );
		for ( int rowIndex = 0; rowIndex < rows.Num(); rowIndex++ )
		{
			FString row	= rows[rowIndex];
			TArray<FString> values	= SplitStringWithDelimiter( row, ",", true );
			for ( int valueIndex = 0; valueIndex < values.Num(); valueIndex++ )
			{
				FString value	= values[valueIndex];
				int count	= CountNumberOfDelimiterInString( value, "'" );
				if ( 2 < count )
				{
					FString tempStr;
					for ( int i = 0; i < count - 1; i++ )
					{
						index	= value.Find( "'", ESearchCase::IgnoreCase, ESearchDir::FromStart );
						tempStr	+= value.Left( index + 1 );
						value.RemoveAt( 0, index + 1 );
						if ( 0 < i )
						{ tempStr	+= "'"; }
					}
					tempStr	+= value;
					values[valueIndex]	= tempStr;
				}
			}

			row.Empty();
			for ( int valueIndex = 0; valueIndex < values.Num(); valueIndex++ )
			{
				FString value	= values[valueIndex];
				row	+= value;
			}
			rows[rowIndex]	= row;
		}

		for ( int rowIndex = 0; rowIndex < rows.Num(); rowIndex++ )
		{
			FString row	= rows[rowIndex];
			toReturn	+= row;
		}
	}
	return  toReturn;
}

UClass* UObjectFactory_SQL::GetBlueprintClass( FString BlueprintPath )
{
	// remove "Blueprints'"
	BlueprintPath.RemoveFromStart( "Blueprint'" );
	// remove ".xxx"
	int index			= BlueprintPath.Find( ".", ESearchCase::IgnoreCase, ESearchDir::FromEnd );
	FString pathToUse	= BlueprintPath.Left( index );
	// add ".uasset" to pathToUse
	pathToUse.Append( ".uasset" );
	const FString FilePath = FPackageName::LongPackageNameToFilename( pathToUse );
	UPackage* Package = FindPackage( NULL, *FilePath );

	//TOFIX Improve as this might be slow to search
	// and not thread safe!


	if ( Package )
	  Package->FullyLoad();
	else
	  Package = LoadPackage( NULL, *FilePath, LOAD_None );

	TArray<UObject*> objects;
	TArray<UClass*> classes;
	if ( Package )
	{
		EngineUtils::EAssetToLoad type	= EngineUtils::ATL_Class;
		ForEachObjectWithOuter( Package, [type, &classes]( UObject* Object )
		{
			if ( Object->IsA<UClass>() )
				classes.Add( ( Cast<UClass>( Object ) ) );
		} );
	}

	return 0 < classes.Num () ? classes[0] : nullptr; 
}

void UObjectFactory_SQL::PopulateSQLiteFields( FString CreateTableQuery )
{
	// Remove whitespaces around "(", ")" and ","
	CreateTableQuery	= RemoveWhiteSpaceAroundDelimiter( CreateTableQuery, "(" );
	CreateTableQuery	= RemoveWhiteSpaceAroundDelimiter( CreateTableQuery, ")" );
	CreateTableQuery	= RemoveWhiteSpaceAroundDelimiter( CreateTableQuery, "," );

	int index	= 0;
	// find the first "("
	index	= CreateTableQuery.Find( "(", ESearchCase::CaseSensitive, ESearchDir::FromStart );//   FindChar( "(", index );
	// remove from start of string to after the first "("
	CreateTableQuery.RemoveAt( 0, index + 1 );
	// find the last ")"
	index	= CreateTableQuery.Find( ")", ESearchCase::CaseSensitive, ESearchDir::FromEnd );//	CreateTableQuery.FindLastChar( *FString( ")" ), index );
	// remove from end of string to after the last ")"
	CreateTableQuery.RemoveAt( index, CreateTableQuery.Len() - index );

	TArray<FString> fields	= SplitStringWithDelimiter( CreateTableQuery, ",", false );
	for ( int i = 0; i < fields.Num(); i++ )
	{
		FString field	= fields[i];
		TArray<FString> fieldData	= SplitStringWithDelimiter( field, " ", false );
		if ( 2 == fieldData.Num() )
		{
			FString fieldName		= fieldData[0];
			FString fieldType		= fieldData[1];
			SQLiteValueTypes type	= SQLiteValueTypes::UNSUPPORTED;

			if ( 0 == fieldType.Compare( "INT" ) )
			{ type = SQLiteValueTypes::INTEGER; }
			else if ( 0 == fieldType.Compare( "FLOAT" ) )
			{ type = SQLiteValueTypes::FLOAT; }
			else if ( 0 == fieldType.Compare( "BOOL" ) )
			{ type = SQLiteValueTypes::BOOL; }
			else if ( 0 == fieldType.Compare( "VARCHAR(255)" ) )
			{ type = SQLiteValueTypes::TEXT; }

			if ( SQLiteValueTypes::UNSUPPORTED != type )
			{
				SQLiteFieldsArray.Add( FSQLiteField( fieldName, type ) );
				SQLiteFieldsMap.Add( fieldName, type );
			} //if ( SQLiteValueTypes::UNSUPPORTED != type )
		} //if ( 2 == fieldData.Num() )
	} //for ( int i = 0; i < fields.Num(); i++ )
}

FString UObjectFactory_SQL::RemoveFromString( FString RemoveFrom, FString ToRemove )
{
	FString toReturn;
	int index	= RemoveFrom.Find( ToRemove, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	while ( -1 != index )
	{
		FString str	= RemoveFrom.Left( index );
		toReturn	+= str;
		RemoveFrom.RemoveAt( 0, index + ToRemove.Len() );
		index		= RemoveFrom.Find( ToRemove, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	} //while ( -1 != index )
	toReturn	+= RemoveFrom;
	return toReturn;
}

FString UObjectFactory_SQL::RemoveWhiteSpaceAroundDelimiter( FString RemoveFrom, FString Delimiter )
{
	FString toReturn;
	int index	= RemoveFrom.Find( Delimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	while ( -1 != index )
	{
		// process left of delimiter
		FString str			= RemoveFrom.Left( index );
		int spaceIndex		= str.Find( " ", ESearchCase::IgnoreCase, ESearchDir::FromEnd );
		bool isWhiteSpace	= str.Len() == spaceIndex + 1;
		while ( isWhiteSpace )
		{
			str.RemoveAt( spaceIndex, 1 );
			spaceIndex		= str.Find( " ", ESearchCase::IgnoreCase, ESearchDir::FromEnd );
			isWhiteSpace	= str.Len() == spaceIndex + 1;
		} //while ( isWhiteSpace )
		// append result to toReturn
		toReturn	+= str + Delimiter;
		
		// process right of delimiter
		RemoveFrom.RemoveAt( 0, index + Delimiter.Len() );
		spaceIndex		= RemoveFrom.Find( " ", ESearchCase::IgnoreCase, ESearchDir::FromStart );
		isWhiteSpace	= 0 == spaceIndex;
		while ( isWhiteSpace )
		{
			RemoveFrom.RemoveAt( spaceIndex, 1 );
			spaceIndex		= RemoveFrom.Find( " ", ESearchCase::IgnoreCase, ESearchDir::FromStart );
			isWhiteSpace	= 0 == spaceIndex;
		} //while ( isWhiteSpace )

		// search for next delimiter
		index	= RemoveFrom.Find( Delimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	} //while ( -1 != index )
	toReturn	+= RemoveFrom;
	return toReturn;
}

TArray<FString> UObjectFactory_SQL::SplitStringWithDelimiter( FString ToSplit, FString Delimiter, const bool KeepDelimiter )
{
	FString leftover	= ToSplit;
	TArray<FString> toReturn;
	int index	= leftover.Find( Delimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	while ( -1 != index )
	{
		FString	strFound	= "";
		// Delimiter will be attached to left side of 
		if ( KeepDelimiter )
		{ strFound	= leftover.Left( index + Delimiter.Len() ); }
		else
		{ strFound	= leftover.Left( index ); }
		toReturn.Add( strFound );
		leftover.RemoveAt( 0, index + Delimiter.Len() );
		index				= leftover.Find( Delimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart );
	} //while ( -1 != index )
	if ( !leftover.IsEmpty() )
	{ toReturn.Add( leftover ); }
	return toReturn;
}

#include "ConstructorHelpers.h"
TSubclassOf<UObject> UObjectFactory_SQL::TestFunc( FName Name, const TCHAR* Path )
{
	//TMap<FName, TSubclassOf<class UObject>> Classes;
	ConstructorHelpers::FClassFinder<UObject> BP( Path );
	//Classes.Add( Name, BP.Class );
	return BP.Class;
}
