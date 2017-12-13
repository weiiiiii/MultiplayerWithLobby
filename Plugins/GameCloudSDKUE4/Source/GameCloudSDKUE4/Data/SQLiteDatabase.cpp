// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "SQLiteDatabase.h"
// include STL
// includes Engine files
#include "StructOnScope.h"
#include "UObject/NoExportTypes.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Data/SQLiteBlueprintNodes.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

#define LOGSQLITE(verbosity, text) UCommonFunctions::PrintToLog( text )// UE_LOG(LogDatabase, verbosity, TEXT("SQLite: %s"), text)

USQLiteDatabase::USQLiteDatabase( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
	, SQLite3Database( nullptr )
	, Tables()
{
	sqlite3_open( ":memory:", &SQLite3Database );
}

void USQLiteDatabase::BeginDestroy()
{
	Super::BeginDestroy();
	sqlite3_close( SQLite3Database );
}

//FSQLiteTable USQLiteDatabase::CreateTable( const FString TableName, const TArray<FSQLiteTableField> Fields, const FSQLitePrimaryKey PK )
//{
//	FSQLiteTable t;
//	t.TableName		= TableName;
//	t.Fields		= Fields;
//	t.PK			= PK;
//
//	FString query = "";
//	query			+= "CREATE TABLE IF NOT EXISTS ";
//	query			+= TableName;
//	query			+= "(";
//
//	bool singlePrimaryKeyExists = false;
//
//	for ( const FSQLiteTableField& field : Fields )
//	{ 
//		if ( 2 < field.ResultStr.Len() )
//		{
//			if ( field.ResultStr.Contains( "PRIMARY KEY" ) )
//			{ singlePrimaryKeyExists = true; }
//			query	+= field.ResultStr + ", ";
//		} //if ( 2 < field.ResultStr.Len() )
//	} //for ( const FSQLiteTableField& field : Fields )
//
//	if ( singlePrimaryKeyExists )
//	{
//		query	= query.Left( query.Len() - 2 );
//		query	+= ");";
//	}
//	else
//	{
//		if ( 2 < PK.ResultStr.Len() )
//		{ query	+= " " + PK.ResultStr + " "; }
//		else
//		{ query = query.Left( query.Len() - 2 ); }
//		query	+= ");";
//	}
//	t.Created	= ExecSql( query );
//	return t;
//}

//bool USQLiteDatabase::CreateTable( const FString TableName, UClass* Class )
//bool USQLiteDatabase::CreateTable( const FString TableName, UStruct* Struct )//const
bool USQLiteDatabase::CreateTable( const FString TableName, TArray<FSQLiteField> TableFields )
{
	bool result	= false;
	
	// create sql, standard opening for creating a table
	FString sql	= "CREATE TABLE IF NOT EXISTS " + TableName + "(";

	for ( int i = 0; i < TableFields.Num(); i++ )
	{
		FSQLiteField field	= TableFields[i];
		FString fieldSQL	= field.Name + " ";
		bool isFieldValid	= true;
		switch ( field.Type )
		{
			case SQLiteValueTypes::BOOL:
				fieldSQL	+= "INTEGER,";
				break;
			case SQLiteValueTypes::INTEGER:
				fieldSQL	+= "INTEGER,";
				break;
			case SQLiteValueTypes::FLOAT:
				fieldSQL	+= "REAL,";
				break;
			case SQLiteValueTypes::TEXT:
				fieldSQL	+= "TEXT,";
				break;
			default:
				isFieldValid	= false;
				break;
		}

		if ( isFieldValid )
		{ sql	+= fieldSQL; }
	}

	// complete sql
	sql	= sql.Left( sql.Len() - 1 );
	sql			+= ");";

	// execute sql
	if ( ExecSql( sql ) )
	{
		// add table to tables if successfully created
		Tables.Add( TableName, TableFields );
		result	= true;
	}

	return result;
}

bool USQLiteDatabase::CreateTableFromSQL( FString SQLQuery )
{
	// format SQLQuery
	// replace MrDataConverter to table name
	// remove \r and \n
	// split into CREATE TABLE and INSERT INTO





	FString modifiedSQLQuery	= RemoveEscapeSequence( SQLQuery, "\r" );
	modifiedSQLQuery			= RemoveEscapeSequence( modifiedSQLQuery, "\n" );

	TArray<FString> SQLQueries	= SeperateQueries( modifiedSQLQuery );
	
	bool result	= true;
	for ( int i = 0; i < SQLQueries.Num(); i++ )
	{
		FString sqlQuery	= SQLQueries[i];
		if ( !ExecSql( sqlQuery ) )
		{ result	= false; }
	}

	return result;
}

bool USQLiteDatabase::ExecSql( const FString Query )
{
	bool execStatus		= false;
	char *zErrMsg		= 0;
	if ( SQLITE_OK == sqlite3_exec( SQLite3Database, TCHAR_TO_UTF8( *Query ), NULL, 0, &zErrMsg ) )
	{ execStatus = true; }
	else
	{ LOGSQLITE( Warning, TEXT( "Query Exec Failed : " + Query ) ); }
	return execStatus;
}

FSQLiteQueryResult USQLiteDatabase::GetData( const FString& Query )
 {
	FSQLiteQueryResult result;
	SQLiteQueryResult queryResult	= RunQueryAndGetResults( Query );
	result.Success					= queryResult.Success;
	result.ErrorMessage				= queryResult.ErrorMessage;

	for ( SQLiteResultRow resultRow : queryResult.Results )
	{
		FSQLiteRow outRow;
		for( TMap<FString, SQLiteResultValue>::TConstIterator resultValueIter = resultRow.Values.CreateConstIterator(); resultValueIter; ++resultValueIter )
		{
			SQLiteResultValue resultValue	= resultValueIter.Value();
			outRow.Cells.Add( resultValueIter.Key(), resultValue.ToString() );
		}
		result.ResultRows.Add( outRow );
	} //for ( auto row : queryResult.Results )

	return result;
}

FSQLiteQueryResult USQLiteDatabase::GetDataBP( TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, int32 MaxResults, int32 ResultOffset )
{
	FSQLiteQueryResult result;
	if ( 0 < Tables.Num() )
	{
		if ( 0 < Fields.Num() )
		{
			FString constructedQuery	= ConstructQuery( Tables, Fields, Query, MaxResults, ResultOffset );
			result						= GetData( constructedQuery );
		}
		else
		{
			LOGSQLITE( Error, TEXT( "The query needs fields! You can use * to get all fields." ) );
			result.Success = false;
			result.ErrorMessage = TEXT( "No fields given" );
		}
	} //if ( 0 < DataSource.Tables.Num() )
	else
	{
		LOGSQLITE( Error, TEXT( "The query needs at least one table name!" ) );
		result.Success = false;
		result.ErrorMessage = TEXT( "No table given" );
	}
	return result;
}

bool USQLiteDatabase::GetDataIntoObject( const FString& Query, UObject* ObjectToPopulate )
{
	bool result	= false;
	if ( NULL != ObjectToPopulate )
	{
		SQLiteQueryResult queryResult	= RunQueryAndGetResults( Query );
		if ( queryResult.Success )
		{
			if ( 0 < queryResult.Results.Num() )
			{
				AssignResultsToObjectProperties( queryResult.Results[0], ObjectToPopulate );
				result	= true;
			}
			else
			{ LOGSQLITE( Error, TEXT( "Query returned zero rows, no data to assign to object properties." ) ); }
		} //if ( queryResult.Success )
		else
		{ LOGSQLITE( Error, *FString::Printf( TEXT( "Query resulted in an error: '%s'" ), *queryResult.ErrorMessage ) ); }
	} //if ( NULL != ObjectToPopulate )
	else
	{ LOGSQLITE( Error, TEXT( "ObjectToPopulate needs to be set to get any results!" ) ); }

	return result;
}

bool USQLiteDatabase::GetDataIntoObjectBP( TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery Query, UObject* ObjectToPopulate )
{
	bool result	= false;

	// Check input validness of ObjectToPopulate
	if ( NULL != ObjectToPopulate )
	{
		// Check input validness of DataSource
		if ( 0 < Tables.Num() )
		{
			// Check input validness of Fields
			if ( 0 < Fields.Num() )
			{
				// Get the results
				FString constructedQuery		= ConstructQuery( Tables, Fields, Query, 1, 0 );
				SQLiteQueryResult queryResult	= RunQueryAndGetResults( constructedQuery );

				if ( queryResult.Success )
				{
					if ( 0 < queryResult.Results.Num() )
					{
						AssignResultsToObjectProperties( queryResult.Results[0], ObjectToPopulate );
						result	= true;
					} //if ( 0 < queryResult.Results.Num() )
					else
					{ LOGSQLITE( Error, TEXT( "Query returned zero rows, no data to assign to object properties." ) ); }
				} //if ( queryResult.Success )
				else
				{ LOGSQLITE( Error, *FString::Printf( TEXT( "Query resulted in an error: '%s'" ), *queryResult.ErrorMessage ) ); }
			} //if ( 0 < Fields.Num() )
			else
			{ LOGSQLITE( Error, TEXT( "The query needs fields! You may use * to get all fields." ) ); }

		} //if ( 0 < DataSource.Tables.Num() )
		else
		{ LOGSQLITE( Error, TEXT( "The query needs the table name!" ) ); }

	} //if ( NULL != ObjectToPopulate )
	else
	{ LOGSQLITE( Error, TEXT( "ObjectToPopulate needs to be set to get any results!" ) ); }

	return result;
}

FSQLiteRow USQLiteDatabase::GetRow( const FString& TableName, const FString& FieldName, const FString& FieldValueToMatch )
{
	// construct sql
	FString sql;
	// getting the whole row, so * wildcard is used
	sql	+= "SELECT *";
	// querying from table, TableName
	sql	+= " FROM " + TableName;
	// FieldName must contain value FieldValueToMatch
	sql	+= " WHERE " + FieldName + " = " + FieldValueToMatch;
	// complete sql
	sql	+= ";";
	// get data from database
	FSQLiteQueryResult queryResult	= GetData( sql );

	FSQLiteRow toReturn;
	if ( queryResult.Success )
	{
		if ( 1 == queryResult.ResultRows.Num() )
		{ toReturn	= queryResult.ResultRows[0]; }
	}
	return toReturn;
}

TArray<FSQLiteRow> USQLiteDatabase::GetRows( const FString& TableName, const TArray<FString> FieldNames, const TArray<FString> FieldValuesToMatch )
{
	TArray<FSQLiteRow> toReturn;
	if ( ensureAlwaysMsgf( FieldNames.Num() == FieldValuesToMatch.Num(), *( FString( "Number of FieldNames is not equal to number of FieldValuesToMatch!" ) ) ) )
	{
		// construct sql
		FString sql;
		// getting the whole row, so * wildcard is used
		sql	+= "SELECT *";
		// querying from table, TableName
		sql	+= " FROM " + TableName;
		// prepare WHERE statement
		sql	+= " WHERE ";
		for ( int i = 0; i < FieldNames.Num(); i++ )
		{
			FString fieldName	= FieldNames[i];
			FString fieldValue	= FieldValuesToMatch[i];
			sql	+= fieldName + " = " + fieldValue + " AND ";
		}
		// remove extra " AND "
		sql	= sql.Left( sql.Len() - 5 );
		// complete sql
		sql	+= ";";
		// get data from database
		FSQLiteQueryResult queryResult	= GetData( sql );
		if ( queryResult.Success )
		{ toReturn	= queryResult.ResultRows; }
	}
	
	return toReturn;
}

TArray<FSQLiteRow> USQLiteDatabase::GetTable( const FString& TableName )
{
	// construct sql
	FString sql;
	// getting the whole row, so * wildcard is used
	sql	+= "SELECT *";
	// querying from table, TableName
	sql	+= " FROM " + TableName;
	// complete sql
	sql	+= ";";
	// get data from database
	FSQLiteQueryResult queryResult	= GetData( sql );

	TArray<FSQLiteRow> toReturn;
	if ( queryResult.Success )
	{ toReturn	= queryResult.ResultRows; }
	return toReturn;
}

bool USQLiteDatabase::IsTableExists( const FString TableName )
{
	int32 sqlReturnCode			= 0;
	int32* sqlReturnCodePtr		= &sqlReturnCode;
	FString Query				= "SELECT * FROM sqlite_master WHERE type='table' AND name='" + TableName + "';";
	sqlite3_stmt* preparedStatement;
	PrepareStatement( &Query, &sqlReturnCodePtr, &preparedStatement );
	
	sqlReturnCode		= *sqlReturnCodePtr;
	bool tableExists	= false;
	if ( SQLITE_OK == sqlReturnCode )
	{
		for ( sqlReturnCode = sqlite3_step( preparedStatement );
			sqlReturnCode != SQLITE_DONE && sqlReturnCode == SQLITE_ROW;
			sqlReturnCode = sqlite3_step( preparedStatement ) )
		{
			tableExists		= true;
			break;
		}
	}
	else
	{
		const char* errorMessage = sqlite3_errmsg( SQLite3Database );
		FString error = "SQL error: " + FString( UTF8_TO_TCHAR( errorMessage ) );
		LOGSQLITE( Error, *error );
		LOGSQLITE( Error, *FString::Printf( TEXT( "The attempted query was: %s" ), *Query ) );
	}

	// release statement
	sqlite3_finalize( preparedStatement );
	return tableExists;
}

bool USQLiteDatabase::InsertRowIntoTable( const FString TableName, TArray<FString> Data )
{
	bool result	= false;
	TArray<FSQLiteField>* table	= Tables.Find( TableName );
	if ( nullptr != table )
	{
		if ( table->Num() == Data.Num() )
		{
			FString sql	= "INSERT INTO " + TableName + " ( ";
			// add field names
			for ( int i = 0; i < table->Num(); i++ )
			{
				FSQLiteField field	= ( *table )[i];
				sql	+= field.Name + ", ";
			}
			//remove trailing ", "
			sql	= sql.Left( sql.Len() - 2 );
			// add field value
			sql = sql + ") VALUES (";
			for ( int i = 0; i < Data.Num(); i++ )
			{
				FString data		= Data[i];
				FSQLiteField field	= ( *table )[i];
				if ( SQLiteValueTypes::TEXT != field.Type )
				{ sql	= sql + data + ", "; }
				else
				{
					int index			= 0;
					while ( -1 != index )
					{
						int indexFound	= 0;
						indexFound	= data.Find( "'", ESearchCase::IgnoreCase, ESearchDir::FromStart, index );
						if ( -1 != indexFound )
						{
							data.InsertAt( indexFound, "'" );
							index	= indexFound + 2;
							// @ last character of the string!
							if ( data.Len() == index )
							{ break; }
						}
						else
						{ index	= -1; }
					}

					sql	= sql + "'" + data + "', ";
				}
			}
			
			// complete sql
			sql	= sql.Left( sql.Len() - 2 );
			sql			+= ");";

			result	= ExecSql( sql );
		}
	}
	return result;
}

//void USQLiteDatabase::PopulateTable( const FString TableName, TArray<FSQLiteTableRowSimulator> rowsOfFields )
//{
//	for ( FSQLiteTableRowSimulator row : rowsOfFields )
//	{
//		FString query = "INSERT INTO " + TableName + " (";
//		for ( FSQLiteTableField field : row.rowsOfFields )
//		{ query += field.FieldName + ", "; }
//
//		query = query.Left( query.Len() - 2 );
//
//		query = query + ") VALUES (";
//		for ( FSQLiteTableField field : row.rowsOfFields )
//		{
//			if ( field.FieldType.Equals( TEXT( "TEXT" ) ) )
//			{ query = query + "'" + field.FieldValue + "', "; }
//			else
//			{ query = query + field.FieldValue + ", "; }
//		} //for ( FSQLiteTableField field : row.rowsOfFields )
//
//		query = query.Left( query.Len() - 2 );
//		query = query + ");";
//
//		ExecSql( query );
//	} //for ( FSQLiteTableRowSimulator row : rowsOfFields )
//}

void USQLiteDatabase::AssignResultsToObjectProperties( const SQLiteResultRow& ResultRow, UObject* ObjectToPopulate )
{
	auto propertyMap = CollectProperties( ObjectToPopulate );
	for ( TMap<FString, SQLiteResultValue>::TConstIterator valueIter = ResultRow.Values.CreateConstIterator(); valueIter; ++valueIter )
	{
		FString fieldName	= valueIter.Key();
		if ( propertyMap.Contains( fieldName ) )
		{
			UProperty* targetProperty		= propertyMap[fieldName];
			SQLiteResultValue resultValue	= valueIter.Value();

			if ( resultValue.Type == SQLiteResultValueTypes::Integer )
			{
				UInt64Property* int64prop = NULL;
				UIntProperty* int32prop = NULL;
				UInt16Property* int16prop = NULL;
				UInt8Property* int8prop = NULL;
				UBoolProperty* boolProp = NULL;

				if ( ( int64prop = Cast<UInt64Property>( targetProperty ) ) != NULL )
				{
					int64prop->SetPropertyValue_InContainer( ObjectToPopulate, resultValue.IntValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%d'" ), *fieldName, resultValue.IntValue ) );
				}
				else if ( ( int32prop = Cast<UIntProperty>( targetProperty ) ) != NULL )
				{
					int32prop->SetPropertyValue_InContainer( ObjectToPopulate, (int32)resultValue.IntValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%d'" ), *fieldName, resultValue.IntValue ) );
				}
				else if ( ( int16prop = Cast<UInt16Property>( targetProperty ) ) != NULL )
				{
					int16prop->SetPropertyValue_InContainer( ObjectToPopulate, (int16)resultValue.IntValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%d'" ), *fieldName, resultValue.IntValue ) );
				}
				else if ( ( int8prop = Cast<UInt8Property>( targetProperty ) ) != NULL )
				{
					int8prop->SetPropertyValue_InContainer( ObjectToPopulate, (int8)resultValue.IntValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%d'" ), *fieldName, resultValue.IntValue ) );
				}
				else if ( ( boolProp = Cast<UBoolProperty>( targetProperty ) ) != NULL )
				{
					boolProp->SetPropertyValue_InContainer( ObjectToPopulate, resultValue.IntValue > 0 );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%d'" ), *fieldName, resultValue.IntValue ) );
				}
			} //if ( resultValue.Type == SQLiteResultValueTypes::Integer )
			else if ( resultValue.Type == SQLiteResultValueTypes::Float )
			{
				UDoubleProperty* doubleProp = NULL;
				UFloatProperty* floatProp = NULL;
				if ( ( doubleProp = Cast<UDoubleProperty>( targetProperty ) ) != NULL )
				{
					doubleProp->SetPropertyValue_InContainer( ObjectToPopulate, resultValue.DoubleValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%f'" ), *fieldName, resultValue.DoubleValue ) );
				}
				else if ( ( floatProp = Cast<UFloatProperty>( targetProperty ) ) != NULL )
				{
					floatProp->SetPropertyValue_InContainer( ObjectToPopulate, (float)resultValue.DoubleValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%f'" ), *fieldName, resultValue.DoubleValue ) );
				}
			} //else if ( resultValue.Type == SQLiteResultValueTypes::Float )
			else if ( resultValue.Type == SQLiteResultValueTypes::Text )
			{
				UStrProperty* strProp = NULL;
				if ( ( strProp = Cast<UStrProperty>( targetProperty ) ) != NULL )
				{
					strProp->SetPropertyValue_InContainer( ObjectToPopulate, resultValue.StringValue );
					LOGSQLITE( Verbose, *FString::Printf( TEXT( "Property '%s' was set to '%s'" ), *fieldName, *resultValue.StringValue.Mid( 0, 64 ) ) );
				}
			} //else if ( resultValue.Type == SQLiteResultValueTypes::Text )
		} //if ( propertyMap.Contains( fieldName ) )
	} //for ( TMap<FString, SQLiteResultValue>::TConstIterator valueIter = ResultRow.Values.CreateConstIterator(); valueIter; ++valueIter )
}

TMap<FString, UProperty*> USQLiteDatabase::CollectProperties( UObject* SourceObject )
{
	UClass* SourceObjectClass = SourceObject->GetClass();
	TMap<FString, UProperty*> Props;
	for ( TFieldIterator<UProperty> PropIt( SourceObjectClass, EFieldIteratorFlags::SuperClassFlags::IncludeSuper );
		PropIt; ++PropIt )
	{
		UProperty* uProperty = *PropIt;
		Props.Add( uProperty->GetNameCPP(), uProperty );
	}

	return Props;
}

TMap<FString, UProperty*> USQLiteDatabase::CollectProperties( UClass* Class )
{
	TMap<FString, UProperty*> Props;
	for ( TFieldIterator<UProperty> PropIt( Class, EFieldIteratorFlags::SuperClassFlags::IncludeSuper );
		PropIt; ++PropIt )
	{
		UProperty* uProperty = *PropIt;
		Props.Add( uProperty->GetNameCPP(), uProperty );
	}

	return Props;
}

TMap<FString, UProperty*> USQLiteDatabase::CollectProperties( UStruct* Struct )
{
	TMap<FString, UProperty*> Props;
	for ( TFieldIterator<UProperty> PropIt( Struct, EFieldIteratorFlags::SuperClassFlags::IncludeSuper );
		PropIt; ++PropIt )
	{
		UProperty* uProperty		= *PropIt;
		//FText displayName			= uProperty->GetDisplayNameText();		// GetDisplayNameText() is only for editor
		//FString nameString			= displayName.ToString();
		FName gibberishName			= uProperty->GetFName();
		FString displayName			= Struct->PropertyNameToDisplayName( gibberishName );
		Props.Add( displayName, uProperty );
	}

	return Props;
}

FString USQLiteDatabase::ConstructQuery( TArray<FString> Tables, TArray<FString> Fields, FSQLiteQueryFinalizedQuery QueryObject, int32 MaxResults /* = -1 */, int32 ResultOffset /* = 0 */ )
{
	FString fieldString;
	for ( int32 i = 0; i < Fields.Num(); i++ )
	{ fieldString.Append( Fields[i] + ( i < Fields.Num() - 1 ? "," : "" ) ); }

	FString tableList = FString::Join( Tables, TEXT( "," ) );
	TArray<FString> allQueryParams;

	allQueryParams.Add( FString::Printf( TEXT( "SELECT %s FROM %s" ), *fieldString, *tableList ) );

	if ( QueryObject.Query.Len() > 0 )
	{ allQueryParams.Add( FString::Printf( TEXT( "WHERE %s" ), *QueryObject.Query ) ); }

	if ( MaxResults >= 0 )
	{ allQueryParams.Add( FString::Printf( TEXT( "LIMIT %i" ), MaxResults ) ); }

	if ( ResultOffset > 0 )
	{ allQueryParams.Add( FString::Printf( TEXT( "OFFSET %i" ), ResultOffset ) ); }

	FString finalQuery = FString::Join( allQueryParams, TEXT( " " ) );
	return finalQuery;
}

void USQLiteDatabase::PrepareStatement( const FString* Query, int32** SqlReturnCode, sqlite3_stmt** PreparedStatement )
{
	ANSICHAR* queryAsUtf8 = TCHAR_TO_UTF8( **Query );
	**SqlReturnCode = sqlite3_prepare_v2( SQLite3Database, queryAsUtf8, -1, PreparedStatement, NULL );
}

FString USQLiteDatabase::RemoveEscapeSequence( FString SQLQuery, FString EscapeSequence )
{
	FString toReturn	= "";
	int index	= SQLQuery.Find( EscapeSequence, ESearchCase::IgnoreCase, ESearchDir::FromStart, 0 );
	while ( -1 != index )
	{
		FString str	= SQLQuery.Left( index );
		toReturn	+= str;
		SQLQuery.RemoveAt( 0, index + EscapeSequence.Len() );
		index		= SQLQuery.Find( EscapeSequence, ESearchCase::IgnoreCase, ESearchDir::FromStart, 0 );
	}
	toReturn	+= SQLQuery;
	return toReturn;
}

FString USQLiteDatabase::RemoveNewLine( FString SQLQuery )
{
	FString toReturn	= "";
	int index	= SQLQuery.Find( "\r\n", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0 );
	while ( -1 != index )
	{
		FString str	= SQLQuery.Left( index );
		toReturn	+= str;
		SQLQuery.RemoveAt( 0, index + 4 );
		index		= SQLQuery.Find( "\r\n", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0 );
	}
	toReturn	+= SQLQuery;
	return toReturn;
}

TArray<FString> USQLiteDatabase::SeperateQueries( FString CombinedSQLQuery )
{
	TArray<FString> toReturn;
	int index	= 0;
	index		= CombinedSQLQuery.Find( ";", ESearchCase::IgnoreCase, ESearchDir::FromStart, index );
	while ( -1 != index )
	{
		// ";" is to be included in this SQLQuery
		index	+= 1;
		FString	sqlQuery	= CombinedSQLQuery.Left( index );
		toReturn.Add( sqlQuery );
		CombinedSQLQuery.RemoveAt( 0, index );
		index				= CombinedSQLQuery.Find( ";", ESearchCase::IgnoreCase, ESearchDir::FromStart, index );
	}
	return toReturn;
}

SQLiteQueryResult USQLiteDatabase::RunQueryAndGetResults( FString Query )
{
	SQLiteQueryResult result;

	int32 sqlReturnCode			= 0;
	int32* sqlReturnCodePtr		= &sqlReturnCode;
	sqlite3_stmt* preparedStatement;

	PrepareStatement( &Query, &sqlReturnCodePtr, &preparedStatement );
	sqlReturnCode = *sqlReturnCodePtr;

	if ( SQLITE_OK == sqlReturnCode )
	{
		//////////////////////////////////////////////////////////////////////////
		// Get and assign the data
		//////////////////////////////////////////////////////////////////////////
		TArray<SQLiteResultRow> resultRows;

		for ( sqlReturnCode = sqlite3_step( preparedStatement ); sqlReturnCode != SQLITE_DONE && sqlReturnCode == SQLITE_ROW; sqlReturnCode = sqlite3_step( preparedStatement ) )
		{
			SQLiteResultRow row;

			LOGSQLITE( Verbose, TEXT( "Query returned a result row." ) );
			int32 resultColumnCount = sqlite3_column_count( preparedStatement );
			for ( int32 c = 0; c < resultColumnCount; c++ )
			{
				int32 columnType		= sqlite3_column_type( preparedStatement, c );
				if ( SQLITE_NULL != columnType )
				{
					SQLiteResultValue resultValue;
					switch ( columnType )
					{
						case SQLITE_INTEGER:
							resultValue.Type = SQLiteResultValueTypes::Integer;
							resultValue.IntValue = sqlite3_column_int64( preparedStatement, c );
							break;
						case SQLITE_TEXT:
							resultValue.Type = SQLiteResultValueTypes::Text;
							resultValue.StringValue = UTF8_TO_TCHAR( sqlite3_column_text( preparedStatement, c ) );
							break;
						case SQLITE_FLOAT:
							resultValue.Type = SQLiteResultValueTypes::Float;
							resultValue.DoubleValue = sqlite3_column_double( preparedStatement, c );
							break;
						default:
							break;
					}

					const char* columnName	= sqlite3_column_name( preparedStatement, c );
					FString columnNameStr	= UTF8_TO_TCHAR( columnName );
					row.Values.Add( columnNameStr, resultValue );
				} //if ( SQLITE_NULL != columnType )
			} //for ( int32 c = 0; c < resultColumnCount; c++ )
			resultRows.Add( row );
		} //for ( sqlReturnCode = sqlite3_step( preparedStatement ); sqlReturnCode != SQLITE_DONE && sqlReturnCode == SQLITE_ROW; sqlReturnCode = sqlite3_step( preparedStatement ) )
		result.Results = resultRows;
		result.Success = true;
	} //if ( sqlReturnCode == SQLITE_OK )
	else
	{
		const char* errorMessage	= sqlite3_errmsg( SQLite3Database );
		FString error				= "SQL error: " + FString( UTF8_TO_TCHAR( errorMessage ) );
		result.ErrorMessage			= error;
		result.Success				= false;
		LOGSQLITE( Error, *error );
		LOGSQLITE( Error, *FString::Printf( TEXT( "The attempted query was: %s" ), *Query ) );		
	}

	// Release the statement and close the connection
	sqlite3_finalize( preparedStatement );
	return result;
}
