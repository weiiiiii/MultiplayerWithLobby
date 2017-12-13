#pragma once
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
//#include "StaticJSON/staticjson.hpp"
//#include "StaticJSON/unreal_types.h"
// fixed final include
#include "SQLiteDatabaseStructs.generated.h"

UENUM( BlueprintType )
enum class SQLiteValueTypes : uint8
{
	BOOL			UMETA( DisplayName = "Bool" ),
	INTEGER			UMETA( DisplayName = "Integer" ),
	FLOAT			UMETA( DisplayName = "Float" ),
	TEXT			UMETA( DisplayName = "Text" ),
	UNSUPPORTED		UMETA( DisplayName = "Unsupported" )
};

/**	Represents a column header of a SQLite table. */
USTRUCT( BlueprintType, meta = ( Category = "SQLite Database Struct" ) )
struct GAMECLOUDSDKUE4_API FSQLiteField
{
	GENERATED_BODY()

	/** Identifier of column. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString Name;

	/** Type of value the column contains. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		SQLiteValueTypes Type;

	FSQLiteField()
		: Name()
		, Type( SQLiteValueTypes::UNSUPPORTED )
	{}

	FSQLiteField( FString InName, SQLiteValueTypes InType )
		: Name( InName )
		, Type( InType )
	{}

	FSQLiteField( const FSQLiteField& SQLiteField )
		: Name( SQLiteField.Name )
		, Type( SQLiteField.Type )
	{}
};

/**	Represents a row in a SQLite table. */
USTRUCT( BlueprintType, meta = ( Category = "SQLite Database Struct" ) )
struct GAMECLOUDSDKUE4_API FSQLiteRow
{
	GENERATED_BODY()

	/** Map of values in this row. [<key,value> : <identifier of the column, value of row at column>] */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TMap<FString,FString> Cells;

	FSQLiteRow()
		: Cells()
	{}

	FSQLiteRow( TMap<FString, FString> InCells )
		: Cells( InCells )
	{}

	FSQLiteRow( const FSQLiteRow& SQLiteRow )
		: Cells( SQLiteRow.Cells )
	{}
};

/** Represents a SQLite table. */
USTRUCT( BlueprintType, meta = ( Category = "SQLite Database Struct" ) )
struct GAMECLOUDSDKUE4_API FSQLiteTable
{
	GENERATED_BODY()

	/** Name of this table. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString Name;
	/** Fields of this table. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TArray<FSQLiteField> Fields;
	/** Data of this table. */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TArray<FSQLiteRow> Rows;

	FSQLiteTable()
		: Name()
		, Fields()
		, Rows()
	{}

	FSQLiteTable( FString InName, TArray<FSQLiteField> InFields, TArray<FSQLiteRow> InRows )
		: Name( InName )
		, Fields( InFields )
		, Rows( InRows )
	{}

	FSQLiteTable( const FSQLiteTable& SQLiteTable )
		: Name( SQLiteTable.Name )
		, Fields( SQLiteTable.Fields )
		, Rows( SQLiteTable.Rows )
	{}
};

/** Represents result of a sql query. */
USTRUCT( BlueprintType, meta = ( Category = "SQLite Database Struct" ) )
struct GAMECLOUDSDKUE4_API FSQLiteQueryResult
{
	GENERATED_BODY()

	/** Resulting rows from query */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TArray<FSQLiteRow> ResultRows;
	/** Was the query successful? */
	UPROPERTY( BlueprintReadOnly )
		bool Success;
	/** Human readable error message if query was unsuccessful */
	UPROPERTY( BlueprintReadOnly )
		FString ErrorMessage;

	FSQLiteQueryResult()
		: ResultRows()
		, Success( false )
		, ErrorMessage( "Query has not been processed" )
	{}
};
