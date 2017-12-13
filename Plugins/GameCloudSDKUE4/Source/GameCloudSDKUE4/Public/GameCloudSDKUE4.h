// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "ModuleManager.h"
// includes project files
#include "GameCloudSDKUE4_Global.h"
// fixed final include

class FGameCloudSDKUE4Module : public IModuleInterface
{
public:
	UWebSocketBase*															_WebSocket_Client;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};