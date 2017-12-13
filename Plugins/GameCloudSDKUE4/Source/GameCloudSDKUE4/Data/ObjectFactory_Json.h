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
//#include "StaticJSON/staticjson.hpp"
//#include "StaticJSON/unreal_types.h"
// includes project files
#include "GameCloudSDKUE4/Data/SQLiteDatabase.h"
#include "GameCloudSDKUE4/Data/SQLiteDatabaseStructs.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
// fixed final include
#include "ObjectFactory_Json.generated.h"

/**
 * 
 */

UCLASS( ClassGroup = ( GameCloudSDKUE4 ), Blueprintable, BlueprintType )
class GAMECLOUDSDKUE4_API UObjectFactory_Json : public UObject
{
	GENERATED_BODY()

public:
	USQLiteDatabase* SQLiteDatabase;
	FString SQLiteTableName;
	TArray<FSQLiteField> SQLiteFieldsArray;
	TMap<FString, SQLiteValueTypes> SQLiteFieldsMap;

protected:

public:
	UObjectFactory_Json( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );
	/**	To be used in place of usual destructor. */
	virtual void BeginDestroy() override;
	/**	To be overridden by derived factory. */
	virtual void ParseData( FString JsonStr );

	AActor* SpawnActorFromDatabase( FString FieldName, FString IDToSpawn, FString BlueprintPathIdentifier = "BlueprintPath" );

protected:
	//-----------------------------------------------------------------------------------------------------------------//
	/** Prints the names of all UProperties in TemplateStruct */
	template<typename TemplateStruct>
	void DebugPrintUPropertiesName()
	{
		UCommonFunctions::PrintToLog( "=================================" );
		UCommonFunctions::PrintToLog( "    PrintVariablesName()" );
		UScriptStruct* templateStructClass	= TemplateStruct::StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		FStructOnScope scopeStruct( templateStructClass );

		for ( TFieldIterator<UProperty> PropIt( scopeStruct.GetStruct(), EFieldIteratorFlags::SuperClassFlags::IncludeSuper );
			PropIt; ++PropIt )
		{
			UProperty* uProperty	= *PropIt;
			FName gibberishName		= uProperty->GetFName();
			FString displayName		= templateStructClass->PropertyNameToDisplayName( gibberishName );
			std::string nameStdString( TCHAR_TO_UTF8( *displayName ) );
			UCommonFunctions::PrintToLog( "    Display name : " + displayName + "    |    Gibberish name : " + gibberishName.ToString() );
		}
		UCommonFunctions::PrintToLog( "=================================" );
	}
	//-----------------------------------------------------------------------------------------------------------------//
	/**
	*	Generates SQLiteField(s) based on name and type of all UProperties 
	*	in TemplateStruct. Supported types include FString, integer, float 
	*	and bool. Unsupported types will not be added to SQLiteFields.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteFields()
	{
		UScriptStruct* templateStructClass	= TemplateStruct::StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		FStructOnScope scopeStruct( templateStructClass );
		for ( TFieldIterator<UProperty> propertyIter( scopeStruct.GetStruct(), EFieldIteratorFlags::SuperClassFlags::IncludeSuper ); propertyIter; ++propertyIter )
		{
			UProperty* uProperty	= *propertyIter;
			FName gibberishName		= uProperty->GetFName();
			FString propertyName	= templateStructClass->PropertyNameToDisplayName( gibberishName );

			SQLiteValueTypes propertySQLiteValueType	= SQLiteValueTypes::UNSUPPORTED;
			if ( uProperty->IsA( UStrProperty::StaticClass() ) )
			{ propertySQLiteValueType	= SQLiteValueTypes::TEXT; }
			else if ( uProperty->IsA( UIntProperty::StaticClass() ) )
			{ propertySQLiteValueType	= SQLiteValueTypes::INTEGER; }
			else if ( uProperty->IsA( UFloatProperty::StaticClass() ) )
			{ propertySQLiteValueType	= SQLiteValueTypes::FLOAT; }
			else if ( uProperty->IsA( UBoolProperty::StaticClass() ) )
			{ propertySQLiteValueType	= SQLiteValueTypes::BOOL; }

			if ( SQLiteValueTypes::UNSUPPORTED != propertySQLiteValueType )
			{
				;
				SQLiteFieldsArray.Push( FSQLiteField( propertyName, propertySQLiteValueType ) );
				SQLiteFieldsMap.Add( propertyName, propertySQLiteValueType );
			}
		}
	}
	/*
	*	Parses JsonStr using ParseJsonToStdMap and generates a SQLiteTable
	*	in the SQLiteDatabase reference.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteTableWithJsonStrMap( FString JsonStr, FString KeyID = "" )
	{
		std::map<std::string, TemplateStruct> dataMap	= ParseJsonToStdMap<TemplateStruct>( JsonStr, KeyID );
		GenerateSQLiteTableWithStdMap( dataMap );
	}
	/*
	*	Parses JsonStr using ParseJsonToStdVector and generates a SQLiteTable
	*	in the SQLiteDatabase reference.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteTableWithJsonStrVector( FString JsonStr )
	{
		std::vector<TemplateStruct> dataVector	= ParseJsonToStdVector( FString JsonStr );
		GenerateSQLiteTableWithStdMap( dataMap );
	}
	/*
	*	Generates a SQLiteTable in the SQLiteDatabase reference with 
	*	std::map<std::string, TemplateStruct>.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteTableWithStdMap( const std::map<std::string, TemplateStruct> DataMap )
	{
		if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "SQLiteDatabase is not set!" ) ) )
		{
			if ( !SQLiteTableName.IsEmpty() )
			{
				if ( 0 < SQLiteFieldsArray.Num() )
				{
					gameInstance->Database->CreateTable( SQLiteTableName, SQLiteFieldsArray );
					for ( std::map<std::string, TemplateStruct>::const_iterator constIter = DataMap.begin(); constIter != DataMap.end(); ++constIter )
					{
						TemplateStruct templateStruct		= constIter->second;
						TArray<FString> templateDataArray	= GetUPropertyValuesFromStruct( &templateStruct );
						SQLiteDatabase->InsertRowIntoTable( SQLiteTableName, templateDataArray );
					}
				}
			}			
		}
	}
	/*
	*	Generates a SQLiteTable in the SQLiteDatabase reference	with 
	*	std::vector<TemplateStruct>.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteTableWithStdVector( const std::vector<TemplateStruct> DataVector )
	{
		if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "SQLiteDatabase is not set!" ) ) )
		{
			if ( !SQLiteTableName.IsEmpty() )
			{
				if ( 0 < SQLiteFieldsArray.Num() )
				{
					gameInstance->Database->CreateTable( SQLiteTableName, SQLiteFieldsArray );
					for ( TemplateStruct templateStruct : DataVector )
					{
						TArray<FString> templateDataArray	= GetUPropertyValuesFromStruct( &templateStruct );
						SQLiteDatabase->InsertRowIntoTable( SQLiteTableName, templateDataArray );
					}
				}
			}
		}
	}
	/*
	*	Generates a SQLiteTable in the SQLiteDatabase of reference with 
	*	TMap<FString, TemplateStruct>.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteTableWithUnrealMap( const TMap<FString, TemplateStruct> DataMap )
	{
		if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "SQLiteDatabase is not set!" ) ) )
		{
			if ( !SQLiteTableName.IsEmpty() )
			{
				if ( 0 < SQLiteFieldsArray.Num() )
				{
					gameInstance->Database->CreateTable( SQLiteTableName, SQLiteFieldsArray );
					for ( TMap<FString, TemplateStruct>::TConstIterator iter = DataMap.CreateConstIterator(); iter; ++iter )
					{
						TemplateStruct templateStruct		= iter.Value();
						TArray<FString> templateDataArray	= GetUPropertyValuesFromStruct<TemplateStruct>( &templateStruct );
						SQLiteDatabase->InsertRowIntoTable( SQLiteTableName, templateDataArray );
					}
				}
			}
		}
	}
	/*
	*	Generates a SQLiteTable in the SQLiteDatabase of reference with 
	*	TArray<TemplateStruct>.
	*/
	template<typename TemplateStruct>
	void GenerateSQLiteTableWithUnrealMap( const TArray<TemplateStruct> DataArray )
	{
		if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "SQLiteDatabase is not set!" ) ) )
		{
			if ( !SQLiteTableName.IsEmpty() )
			{
				if ( 0 < SQLiteFieldsArray.Num() )
				{
					gameInstance->Database->CreateTable( SQLiteTableName, SQLiteFieldsArray );
					for ( TemplateStruct templateStruct : DataArray )
					{
						TArray<FString> templateDataArray	= GetUPropertyValuesFromStruct<TemplateStruct>( &templateStruct );
						SQLiteDatabase->InsertRowIntoTable( SQLiteTableName, templateDataArray );
					}
				}
			}
		}
	}
	//-----------------------------------------------------------------------------------------------------------------//
	FSQLiteRow GetDataByID( FString ColumnName, FString RowName )
	{
		FSQLiteRow result;
		SQLiteValueTypes* valueType	= SQLiteFieldsMap.Find( ColumnName );
		if ( nullptr != valueType )
		{
			if ( ensureAlwaysMsgf( nullptr != SQLiteDatabase, *FString( "SQLiteDatabase is not set!" ) ) )
			{
				if ( SQLiteValueTypes::TEXT == *valueType )
				{ result	= SQLiteDatabase->GetRow( SQLiteTableName, ColumnName, "'" + RowName + "'" ); }
				else
				{ result	= SQLiteDatabase->GetRow( SQLiteTableName, ColumnName, RowName ); }
			}
		}		
		return result;
	}


	/**
	*	Retrieves value of all UProperties in TemplateStruct, where name of
	*	UProperty matches name of a field in SQLiteFieldsArray, as TArray<FString>.
	*/
	template<typename TemplateStruct>
	TArray<FString> GetUPropertyValuesFromStruct( TemplateStruct* StructPointer )
	{
		TArray<FString> toReturn;
		UScriptStruct* templateStructClass	= StructPointer->StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		for ( FSQLiteField field : SQLiteFieldsArray )
		{
			const TCHAR* charName	= *( field.Name );
			switch ( field.Type )
			{
				case SQLiteValueTypes::BOOL:
				{
					UBoolProperty* uProperty	= FindField<UBoolProperty>( templateStructClass, charName );
					if ( nullptr != uProperty )
					{
						bool value	= uProperty->GetPropertyValue_InContainer( StructPointer );
						toReturn.Push( FString::FromInt( value ) );
					}
					else
					{ UCommonFunctions::PrintToLog( "    " + field.Name + " is not found or is not an UBoolProperty" ); }
				}
				break;
				case SQLiteValueTypes::INTEGER:
				{
					UIntProperty* uProperty		= FindField<UIntProperty>( templateStructClass, charName );
					if ( nullptr != uProperty )
					{
						int value	= uProperty->GetPropertyValue_InContainer( StructPointer );
						toReturn.Push( FString::FromInt( value ) );
					}
					else
					{ UCommonFunctions::PrintToLog( "    " + field.Name + " is not found or is not an UIntProperty" ); }
				}
				break;
				case SQLiteValueTypes::FLOAT:
				{
					UFloatProperty* uProperty	= FindField<UFloatProperty>( templateStructClass, charName );
					if ( nullptr != uProperty )
					{
						float value	= uProperty->GetPropertyValue_InContainer( StructPointer );
						toReturn.Push( FString::SanitizeFloat( value ) );
					}
					else
					{ UCommonFunctions::PrintToLog( "    " + field.Name + " is not found or is not an UFloatProperty" ); }
				}
				break;
				case SQLiteValueTypes::TEXT:
				{
					UStrProperty* uProperty		= FindField<UStrProperty>( templateStructClass, charName );
					if ( nullptr != uProperty )
					{
						FString value	= uProperty->GetPropertyValue_InContainer( StructPointer );
						toReturn.Push( value );
					}
					else
					{ UCommonFunctions::PrintToLog( "    " + field.Name + " is not found or is not an UStrProperty" ); }
				}
				break;
				default:
					break;
			}
		}
		return toReturn;
	}

	TSubclassOf<class UObject> LoadBlueprintClass( const FString BlueprintClassPath )
	{
		const TCHAR* path			= *BlueprintClassPath;
		UObject* tempObject			= StaticLoadObject( UObject::StaticClass(), nullptr, path );
		UBlueprint* blueprintObject = Cast<UBlueprint>( tempObject );
		TSubclassOf<class UObject> blueprintClass;
		blueprintClass				 = (UClass*)blueprintObject->GeneratedClass;

		return blueprintClass;
	}
	//-----------------------------------------------------------------------------------------------------------------//
	/** Parses JsonStr using TemplateStruct. */
	template<typename TemplateStruct>
	FORCEINLINE TemplateStruct ParseJsonToStruct( FString JsonStr )
	{
		std::string jsonStdStr( TCHAR_TO_UTF8( *JsonStr ) );
		TemplateStruct data;
		staticjson::ParseStatus status;
		from_json_string( jsonStdStr.c_str(), &data, &status );
		return data;
	}
	/*
	*	Parses JsonStr using TemplateStruct and returns std::map<std::string,TemplateStruct>.
	*	If KeyID is not empty and there exist an UProperty with the same name
	*	in TemplateStruct, all key values will be assigned to the UProperty
	*	of the corresponding TemplateStructs.
	*/
	template<typename TemplateStruct>
	FORCEINLINE std::map<std::string, TemplateStruct> ParseJsonToStdMap( FString JsonStr, FString KeyID = "" )
	{
		std::string jsonStdStr( TCHAR_TO_UTF8( *JsonStr ) );
		std::map<std::string, TemplateStruct> dataMap;
		staticjson::ParseStatus status;
		from_json_string( jsonStdStr.c_str(), &dataMap, &status );
		for ( std::map<std::string, TemplateStruct>::iterator iter = dataMap.begin(); iter != dataMap.end(); ++iter )
		{
			FString key				= FString( iter->first.c_str() );
			TemplateStruct* value	= &( iter->second );
			if ( !KeyID.IsEmpty() )
			{ SetUStrPropertyValueInStruct<TemplateStruct>( value, KeyID, key ); }
		}
		return dataMap;
	}
	/**	Parses JsonStr using TemplateStruct and returns std::vector<TemplateStruct>. */
	template<typename TemplateStruct>
	FORCEINLINE std::vector<TemplateStruct> ParseJsonToStdVector( FString JsonStr )
	{
		std::string jsonStdStr( TCHAR_TO_UTF8( *JsonStr ) );
		std::vector<TemplateStruct> dataVector;
		staticjson::ParseStatus status;
		from_json_string( jsonStdStr.c_str(), &dataVector, &status );
		return dataVector;
	}
	/*
	*	Parses JsonStr using TemplateStruct and returns TMap<FString,TemplateStruct>.
	*	If KeyID is not empty and there exist an UProperty with the same name
	*	in TemplateStruct, all key values will be assigned to the UProperty
	*	of the corresponding TemplateStructs.
	*/
	template<typename TemplateStruct>
	FORCEINLINE TMap<FString, TemplateStruct> ParseJsonToUnrealMap( FString JsonStr, FString KeyID = "" )
	{
		std::map<std::string, TemplateStruct> dataMap	= ParseJsonToStdMap<TemplateStruct>( JsonStr, KeyID );
		TMap<FString, TemplateStruct> toReturn;
		for ( std::map<std::string, TemplateStruct>::iterator iter = dataMap.begin(); iter != dataMap.end(); ++iter )
		{
			FString key				= FString( iter->first.c_str() );
			TemplateStruct value	= iter->second;
			toReturn.Add( key, value );
		}
		return toReturn;
	}
	/**	Parses JsonStr using TemplateStruct and returns TArray<TemplateStruct>. */
	template<typename TemplateStruct>
	FORCEINLINE TArray<TemplateStruct> ParseJsonToUnrealArray( FString JsonStr )
	{
		std::vector<TemplateStruct> dataVector	= ParseJsonToStdVector( JsonStr );
		TArray<TemplateStruct> toReturn;
		for ( int i = 0; i < dataVector.size(); i++ )
		{
			TemplateStruct value	= dataVector[i];
			toReturn.Add( value );
		}
		return toReturn;
	}
	//-----------------------------------------------------------------------------------------------------------------//
	/**	Sets the value of UBoolProperty as param NewValue, if found in TemplateStruct. */
	template<typename TemplateStruct>
	FORCEINLINE void SetUBoolPropertyValueInStruct( TemplateStruct* StructPointer, FString PropertyName, bool NewValue )
	{
		UScriptStruct* templateStructClass	= StructPointer->StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		const TCHAR* charName				= *( PropertyName );
		UBoolProperty* uProperty			= FindField<UBoolProperty>( templateStructClass, charName );
		if ( nullptr != uProperty )
		{ uProperty->SetPropertyValue_InContainer( StructPointer, NewValue ); }
	}
	/**	Sets the value of UFloatProperty as param NewValue, if found in TemplateStruct. */
	template<typename TemplateStruct>
	FORCEINLINE void SetUFloatPropertyValueInStruct( TemplateStruct* StructPointer, FString PropertyName, float NewValue )
	{
		UScriptStruct* templateStructClass	= StructPointer->StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		const TCHAR* charName				= *( PropertyName );
		UFloatProperty* uProperty			= FindField<UFloatProperty>( templateStructClass, charName );
		if ( nullptr != uProperty )
		{ uProperty->SetPropertyValue_InContainer( StructPointer, NewValue ); }
	}
	/**	Sets the value of UIntProperty as param NewValue, if found in TemplateStruct. */
	template<typename TemplateStruct>
	FORCEINLINE void SetUIntPropertyValueInStruct( TemplateStruct* StructPointer, FString PropertyName, int NewValue )
	{
		UScriptStruct* templateStructClass	= StructPointer->StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		const TCHAR* charName				= *( PropertyName );
		UIntProperty* uProperty				= FindField<UIntProperty>( templateStructClass, charName );
		if ( nullptr != uProperty )
		{ uProperty->SetPropertyValue_InContainer( StructPointer, NewValue ); }
	}
	/**	Sets the value of UStrProperty as param NewValue, if found in TemplateStruct. */
	template<typename TemplateStruct>
	FORCEINLINE void SetUStrPropertyValueInStruct( TemplateStruct* StructPointer, FString PropertyName, FString NewValue )
	{
		UScriptStruct* templateStructClass	= StructPointer->StaticStruct();	// Compilation will fill if TemplateStruct is not a UE4 struct
		const TCHAR* charName				= *( PropertyName );
		UStrProperty* uProperty				= FindField<UStrProperty>( templateStructClass, charName );
		if ( nullptr != uProperty )
		{ uProperty->SetPropertyValue_InContainer( StructPointer, NewValue ); }
	}
};