// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GameCloudSDKUE4 : ModuleRules
{
	public GameCloudSDKUE4( ReadOnlyTargetRules Target ) : base( Target )
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"GameCloudSDKUE4/Public"
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				"GameCloudSDKUE4/Private"
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				, "InputCore"
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject"
				, "Engine"
				, "Slate"
				, "SlateCore"
				, "PhysXVehicles"
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		IncludeSQLite3( Target );
		IncludeStaticJson( Target );
		IncludeWebSocket( Target );
	}

	public string ProjectFolderPath
	{ get { return Path.GetFullPath( Path.Combine( ModuleDirectory, "../" ) ); } }

	public string ThirdPartyPath
	{ get { return Path.GetFullPath( Path.Combine( ProjectFolderPath, "ThirdParty" ) ); } }

	public void IncludeStaticJson( ReadOnlyTargetRules Target )
	{
		PublicDependencyModuleNames.Add( "StaticJSON" );
		PrivateDependencyModuleNames.Add( "StaticJSON" );
		PublicIncludePaths.Add( "GameCloudSDKUE4/External/StaticJson" );
		PrivateIncludePaths.Add( "GameCloudSDKUE4/External/StaticJson" );
	}

	public void IncludeSQLite3( ReadOnlyTargetRules Target )
	{
		PublicDependencyModuleNames.Add( "SQLite3" );
		PrivateDependencyModuleNames.Add( "SQLite3" );
	}

	public void IncludeWebSocket( ReadOnlyTargetRules Target )
	{
		// dependency required for converting data to and from websocket server
		PublicDependencyModuleNames.Add( "BinStream" );
		PrivateDependencyModuleNames.Add( "BinStream" );

		PublicDependencyModuleNames.Add( "Json" );
		PrivateDependencyModuleNames.Add( "Json" );
		PublicDependencyModuleNames.Add( "JsonUtilities" );
		PrivateDependencyModuleNames.Add( "JsonUtilities" );

		PrivateDependencyModuleNames.Add( "zlib" );
		PrivateDependencyModuleNames.Add( "OpenSSL" );

		// websocket path
		string webSocketPath	= Path.Combine( ThirdPartyPath, "WebSocket" );
		// add include path
		string includePath		= Path.Combine( webSocketPath, "include/Win64" );
		PublicIncludePaths.Add( includePath );
		// add library path
		string libraryPath		= Path.Combine( webSocketPath, "lib/Win64" );
		PublicLibraryPaths.Add( libraryPath );
		// add library
		PublicAdditionalLibraries.Add( "websockets_static.lib" );
	}
}
