// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class StaticJSON : ModuleRules
{
	public StaticJSON( ReadOnlyTargetRules Target ) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

		// add include path
		string rapidjsonPath = Path.Combine( ModuleDirectory, "rapidjson" );
		PublicIncludePaths.Add( rapidjsonPath );
		string staticjsonPath = Path.Combine( ModuleDirectory, "StaticJSON" );
		PublicIncludePaths.Add( staticjsonPath );
		// add library
		string libraryPath = Path.Combine( ModuleDirectory, "x64" );
		//		error LNK2038: mismatch detected for '_ITERATOR_DEBUG_LEVEL': value '2' doesn't match value '0' in SharedPCH.Engine.h.obj
		//		https://forums.unrealengine.com/development-discussion/c-gameplay-programming/34307-building-engine-with-debug-runtime-library
		//		https://docs.unrealengine.com/latest/INT/Programming/UnrealBuildSystem/Configuration/
		//		Should this be required, modify Engine/Saved/UnrealBuildTool/BuildConfiguration.xml
		//		<bDebugBuildsActuallyUseDebugCRT>true</bDebugBuildsActuallyUseDebugCRT>
		//if( Target.Configuration == UnrealTargetConfiguration.Debug
		//	|| Target.Configuration == UnrealTargetConfiguration.DebugGame
		//	|| Target.Configuration == UnrealTargetConfiguration.Development )
		//{ libraryPath = Path.Combine( libraryPath, "Debug/StaticJSON.lib" ); }
		//else
		{ libraryPath = Path.Combine( libraryPath, "Release/StaticJSON.lib" ); }
		PublicAdditionalLibraries.Add( libraryPath );
	}
}
