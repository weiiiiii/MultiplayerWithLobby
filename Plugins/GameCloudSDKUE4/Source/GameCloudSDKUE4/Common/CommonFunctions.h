// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <string>
// includes Engine files
#include "Kismet/BlueprintFunctionLibrary.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "CommonFunctions.generated.h"

/**
 * 
 */
// There are no blueprint callable functions here, so consider making this a normal c++ class instead
UCLASS()
class GAMECLOUDSDKUE4_API UCommonFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template<typename TEnum>
	static FString GetEnumValueToString( const FString& Name, TEnum Value )
	{
		const UEnum* enumPtr = FindObject<UEnum>( ANY_PACKAGE, *Name, true );
		if ( !enumPtr )
		{
			return FString( "Invalid" );
		}
		return enumPtr->GetNameByValue( (int64)Value ).ToString();
	}

	// any value that is not -1 or 18,446,744,073,709,551,615 or 0xFFFFFFFFFFFFFFFF will attempt to replace existing message
	static void PrintStringOnScreen( float TimeToDisplay, FColor DisplayColour, const std::string& DebugMessage, uint64 key = -1 );
	// any value that is not -1 or 18,446,744,073,709,551,615 or 0xFFFFFFFFFFFFFFFF will attempt to replace existing message
	static void PrintFStringOnScreen( float TimeToDisplay, FColor DisplayColour, const FString& DebugMessage, uint64 key = -1 );
	static void PrintToLog( const FString& DebugMessage );
	
};
