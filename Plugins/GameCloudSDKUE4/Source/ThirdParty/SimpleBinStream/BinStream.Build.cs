// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BinStream : ModuleRules
{
	public BinStream( ReadOnlyTargetRules Target ) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

		// add include path
		PublicIncludePaths.Add( Path.Combine( ModuleDirectory, "CustomOperators.h" ) );
		PublicIncludePaths.Add( Path.Combine( ModuleDirectory, "SimpleBinStream.h" ) );
	}
}
