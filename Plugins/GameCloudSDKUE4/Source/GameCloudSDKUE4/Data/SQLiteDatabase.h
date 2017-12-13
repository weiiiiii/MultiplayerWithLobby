// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
// include plugin files
// includes third party files
#include "sqlite3.h"
// includes project files
#include "GameCloudSDKUE4/Data/SQLiteBlueprintNodes.h"
#include "GameCloudSDKUE4/Data/SQLiteDatabaseStructs.h"
// fixed final include
#include "SQLiteDatabase.generated.h"


// A few things for internal use here.
namespace SQLiteResultValueTypes
{
	enum SQLiteResultValType
	{
		Integer,
		Float,
		Text,
		UnsupportedValueType
	};
}

// Result field, used as an intermediary when collecting results from
// an SQLITE3 query.
struct SQLiteResultValue
{
	FString StringValue;
	double DoubleValue;
	int64 IntValue;

	SQLiteResultValueTypes::SQLiteResultValType Type;

	SQLiteResultValue()
		: StringValue()
		, DoubleValue( 0.0f )
		, IntValue( 0 )
		, Type( SQLiteResultValueTypes::UnsupportedValueType )
	{}

	FString ToString()
	{
		if ( Type == SQLiteResultValueTypes::Text )
			return StringValue;
		else if ( Type == SQLiteResultValueTypes::Integer )
			return FString::Printf( TEXT( "%i" ), IntValue );
		else if ( Type == SQLiteResultValueTypes::Float )
			return FString::Printf( TEXT( "%f" ), DoubleValue );

		return StringValue;
	}
};

// Represents a single row in the result.
struct SQLiteResultRow
{
	TMap<FString,SQLiteResultValue> Values;
	//TArray<SQLiteResultValue> Fields;
};

// The internal result object.
struct SQLiteQueryResult
{
	bool Success;
	FString ErrorMessage;
	TArray<SQLiteResultRow> Results;
};

/**
* SQLite database class. Contains a database stored in memory.
*/
UCLASS( ClassGroup = ( GameCloudSDKUE4 ), Blueprintable, BlueprintType )
class GAMECLOUDSDKUE4_API USQLiteDatabase : public UObject
{
	GENERATED_BODY()
public:
protected:
	sqlite3* SQLite3Database;
	/** List of tables in this database */
	TMap<FString, TArray<FSQLiteField>> Tables;

public:
	USQLiteDatabase( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );
	virtual void BeginDestroy() override;

	/** Create table in the database. */
	UFUNCTION( BlueprintCallable, Category = "SQLite|Query", meta = ( DisplayName = "Create Table" ) )
	//	FSQLiteTable CreateTable( const FString TableName, const TArray<FSQLiteTableField> Fields, const FSQLitePrimaryKey PK );
		//bool CreateTable( const FString TableName, UClass* Class );
		//bool CreateTable( const FString TableName, UStruct* TableFields );
		bool CreateTable( const FString TableName, TArray<FSQLiteField> TableFields );
	//bool CreateTable2( const FString TableName, TArray<FSQLiteField> TableFields,  );

	UFUNCTION( BlueprintCallable, Category = "SQLite|Query", meta = ( DisplayName = "Create Table with SQL" ) )
		bool CreateTableFromSQL( const FString SQLQuery );

	/** Execute SQL (can be used for insert statement)*/
	UFUNCTION( BlueprintCallable, Category = "SQLite|Query", meta = ( DisplayName = "Execute SQL Query" ) )
		bool ExecSql( const FString Query );

	/** Get data from the database using a select statement and return the rows. */
	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Data From Table(s) (manual query)" ) )
		FSQLiteQueryResult GetData( const FString& Query );

	/** Blueprint: Get data from the database. Returns the resulting rows. */
	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Data From Table(s)" ) )
		FSQLiteQueryResult GetDataBP( TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, int32 MaxResults = -1, int32 ResultOffset = 0 );

	/** Get data from the database using a select statement straight into an UObject, ie. populates its properties. */
	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Data Into Object (manual query)" ) )
		bool GetDataIntoObject( const FString& Query, UObject* ObjectToPopulate );

	/** Blueprint: Gets data from the database using a select statement straight into an UObject, ie. populates its properties.
	*   Note: Does not create a new object. ObjectToPopulate is the reference to the object you want to populate. */
	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Data Into Object" ) )
		bool GetDataIntoObjectBP( TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, UObject* ObjectToPopulate );

	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Row" ) )
		FSQLiteRow GetRow( const FString& TableName, const FString& FieldName, const FString& FieldValueToMatch );

	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Row" ) )
		TArray<FSQLiteRow> GetRows( const FString& TableName, const TArray<FString> FieldNames, const TArray<FString> FieldValuesToMatch );

	UFUNCTION( BlueprintCallable, Category = "SQLite", meta = ( DisplayName = "Get Table" ) )
		TArray<FSQLiteRow> GetTable( const FString& TableName );

	/** Is table exists?*/
	UFUNCTION( BlueprintCallable, Category = "SQLite|Query", meta = ( DisplayName = "Is table exists?" ) )
		bool IsTableExists( const FString TableName );

	/** Insert rows into table, returns true if data is successfully inserted */
	UFUNCTION( BlueprintCallable, Category = "SQLite|Query", meta = ( DisplayName = "Insert Row Into Table" ) )
		bool InsertRowIntoTable( const FString TableName, TArray<FString> Data );
		//void PopulateTable( const FString TableName, TArray<FSQLiteTableRowSimulator> rowsOfFields );


protected:
	/** Assigns a result row's fields' values to an UObject, ie. assigns them to the properties that have the same name. */
	void AssignResultsToObjectProperties( const SQLiteResultRow& ResultValue, UObject* ObjectToPopulate );

	/** Collects all properties from an UObject and maps them by the property name. */
	TMap<FString, UProperty*> CollectProperties( UObject* SourceObject );
	/** Collects all properties from an UClass and maps them by the property name. */
	TMap<FString, UProperty*> CollectProperties( UClass* Class );
	/** Collects all properties from an UStruct and maps them by the property name. */
	TMap<FString, UProperty*> CollectProperties( UStruct* Struct );

	/** Constructs an SQL query from the blueprint fed data. */
	FString ConstructQuery( TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery QueryObject, int32 MaxResults = -1, int32 ResultOffset = 0 );
	
	void PrepareStatement( const FString* Query, int32** SqlReturnCode, sqlite3_stmt** PreparedStatement );

	FString RemoveEscapeSequence( FString SQLQuery, FString EscapeSequence );
	FString RemoveNewLine( FString SQLQuery );
	TArray<FString> SeperateQueries( FString CombinedSQLQuery );

	/** Runs a query and returns fetched rows. */
	SQLiteQueryResult RunQueryAndGetResults( FString Query );
};
