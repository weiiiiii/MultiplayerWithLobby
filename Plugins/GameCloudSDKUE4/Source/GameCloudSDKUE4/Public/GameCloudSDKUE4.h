// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "ModuleManager.h"
// includes project files
// fixed final include

class FGameCloudSDKUE4Module : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};