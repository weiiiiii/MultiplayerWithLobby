// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GameCloudSDK : ModuleRules
{
	public GameCloudSDK( ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

        string libraryPath = "";
//         if ( Target.Platform == UnrealTargetPlatform.Win32 )
//             libraryPath = Path.Combine( ModuleDirectory, "x86", "Release" );
//         else if ( Target.Platform == UnrealTargetPlatform.Win64 )
            libraryPath = Path.Combine( ModuleDirectory, "x64", "Release" );    // commented out above code because currently we are using only x64 lib and dll

        // Add the import library
        PublicLibraryPaths.Add( libraryPath );
        PublicAdditionalLibraries.Add( "GameCloudSDK.lib" );

        // Delay-load the DLL, so we can load it from the right place first
        PublicDelayLoadDLLs.Add( "GameCloudSDK.dll" );

        // Include header file
        // PublicIncludePaths.Add( "GameCloudSDK.h" );
        //PublicIncludePaths.Add( "GameCloudSDK/Include" );

        // Add runtime dependencies so that DLL will be packaged together
        RuntimeDependencies.Add( new RuntimeDependency( Path.Combine( libraryPath, "GameCloudSDK.dll" ) ) );
    }
}
