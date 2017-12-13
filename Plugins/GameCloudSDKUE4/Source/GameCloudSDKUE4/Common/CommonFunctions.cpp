// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "CommonFunctions.h"
// include STL
// includes Engine files
#include "Core.h"
#include "Engine.h"
// include plugin files
// includes third party files
// includes project files

void UCommonFunctions::PrintStringOnScreen( float TimeToDisplay, FColor DisplayColour, const std::string& DebugMessage, uint64 key )
{
	FString fstr( DebugMessage.c_str() );
	PrintFStringOnScreen( TimeToDisplay, DisplayColour, fstr, key );
}

void UCommonFunctions::PrintFStringOnScreen( float TimeToDisplay, FColor DisplayColour, const FString& DebugMessage, uint64 key )
{
	if ( GEngine )
		GEngine->AddOnScreenDebugMessage( key, TimeToDisplay, DisplayColour, DebugMessage );
}

void UCommonFunctions::PrintToLog( const FString& DebugMessage )
{
	UE_LOG( LogTemp, Warning, TEXT( "%s" ), *DebugMessage );
}
