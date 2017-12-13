// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SQLite3 : ModuleRules
{
	public SQLite3( ReadOnlyTargetRules Target ) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

		// add include path
		string IncludePath = Path.Combine( ModuleDirectory, "Include" );
		PublicIncludePaths.Add( IncludePath );
		// add library
		string libraryPath = Path.Combine( ModuleDirectory, "Lib" );
		libraryPath = Path.Combine( libraryPath, "SQLite3.x64.lib" );
		PublicAdditionalLibraries.Add( libraryPath );
	}
}
