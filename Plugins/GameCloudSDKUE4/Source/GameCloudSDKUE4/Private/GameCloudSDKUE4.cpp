// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

// fixed first include
#include "GameCloudSDKUE4.h"
// include STL
//#include <list>			// Added for GameCloudSDK
//#include <string>			// Added for GameCloudSDK
//#include <windows.h>		// Added for GameCloudSDK
// includes Engine files
#include "Core.h"
#include "ModuleManager.h"
// includes third party files
// includes project files
#include "GameCloudSDKUE4_Global.h"
#include "GameCloudSDKUE4/WebSocket/WebSocketBase.h"
#include "GameCloudSDKUE4/WebSocket/WebSocketBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "FGameCloudSDKUE4Module"

void FGameCloudSDKUE4Module::StartupModule()
{
	check( GConfig );

	//FString basePluginPath = FPaths::GamePluginsDir();
	//FString LibraryPath = FPaths::Combine( *basePluginPath, TEXT( "GameCloudSDKUE4/Source/ThirdParty/GameCloudSDK/x64/Release/GameCloudSDK.dll" ) );
	//LoadLibrary( *LibraryPath );

	check( 0 == _WebSocket_Client );
	_WebSocket_Client	= UWebSocketBlueprintLibrary::Connect( "ws://localhost:9002" );
	//_Vote_WebSocket		= UWebSocketBlueprintLibrary::Connect( "ws://localhost:9001/playerroom1" );
	//_WebSocket_Client	= UWebSocketBlueprintLibrary::Connect( "ws://192.168.1.99:9002" );
	
	//std::list<std::string> relativeLibraryPaths;
	//relativeLibraryPaths.push_back( "GameCloudSDKUE4/Source/ThirdParty/GameCloudSDK/x64/Release/GameCloudSDK.dll" );
	//
	//for ( std::string path : relativeLibraryPaths )
	//{
	//	FString LibraryPath = FPaths::Combine( *basePluginPath, path.c_str() );
	//	LoadLibrary( *LibraryPath );
	//}
}

void FGameCloudSDKUE4Module::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameCloudSDKUE4Module, GameCloudSDKUE4)