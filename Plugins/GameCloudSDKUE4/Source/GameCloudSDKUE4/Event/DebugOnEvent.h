// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
#include <sstream>
// includes Engine files
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
// fixed final include
#include "DebugOnEvent.generated.h"

namespace
{

	void LogMessage( const char * message, float duration, FColor color )
	{
		UCommonFunctions::PrintFStringOnScreen( duration, color, message );
	}
}

USTRUCT( BlueprintType )
struct FDebugOnEvent
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Prints event details in output log" ) )
		bool PrintInLog;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Prints event details in on screen" ) )
		bool PrintOnScreen;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Colour to use when printing on screen" ) )
		FColor ScreenColour;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Tooltip = "Duration message will last on screen" ) )
		float Duration;
	
	FDebugOnEvent()
		: PrintInLog( false )
		, PrintOnScreen( false )
		, ScreenColour( FColor::Red )
		, Duration( 2.0f )
	{
	}

	void PrintOnHitEvent( std::string prefix, const FHitResult& Hit )
	{
		if ( !PrintOnScreen && !PrintInLog )
			return;

		std::ostringstream oss;
		// set prefix and add space
		oss << prefix << " : ";
		// try to add physics material
		if ( nullptr != Hit.PhysMaterial )
		{
			const FString & surfaceTypeName = UCommonFunctions::GetEnumValueToString( "EPhysicalSurface", Hit.PhysMaterial->SurfaceType );
			AppendFString( &oss, "Surface Type", surfaceTypeName );
		}
		// try to add actor name
		AppendName( Hit.GetActor(), &oss, "Actor Name" );
		// try to add component name
		AppendName( Hit.GetComponent(), &oss, "Component Name" );
		// try to add bone name
		AppendFString( &oss, "Bone Name", Hit.BoneName.ToString() );
		FString message = FString( oss.str().c_str() );
		if ( PrintOnScreen )
			UCommonFunctions::PrintFStringOnScreen( Duration, ScreenColour, message );
		if ( PrintInLog )
			UCommonFunctions::PrintToLog( message );
	}

	void PrintMessageOnEvent( FString MessageToPrint )
	{
		if ( !PrintOnScreen && !PrintInLog )
			return;
		if ( PrintOnScreen )
			UCommonFunctions::PrintFStringOnScreen( Duration, ScreenColour, MessageToPrint );
		if ( PrintInLog )
			UCommonFunctions::PrintToLog( MessageToPrint );
	}

	void AppendName( const UObject* object, std::ostringstream* oss, std::string prefix )
	{
		if ( nullptr == object )
			return;

		FString name = UKismetSystemLibrary::GetDisplayName( object );
		if ( name.IsEmpty() )
			return;

		std::string nameStr( TCHAR_TO_UTF8( *name ) );
		(*oss) << prefix << " : [" << nameStr << "] ";
	}

	void AppendFString( std::ostringstream* oss, std::string prefix, FString fstring )
	{
		if ( fstring.IsEmpty() )
			return;

		std::string str( TCHAR_TO_UTF8( *fstring ) );
		( *oss ) << prefix << " : [" << str << "] ";
		const char * message = ( *oss ).str().c_str();
		const char * message2 = ( *oss ).str().c_str();
	}
};


