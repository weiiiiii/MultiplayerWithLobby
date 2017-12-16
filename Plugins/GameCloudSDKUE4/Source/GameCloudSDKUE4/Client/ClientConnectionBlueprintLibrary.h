// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "SimpleBinStream.h"

// include STL
// includes Engine files
#include "Kismet/BlueprintFunctionLibrary.h"
// includes plugin files
// includes third party files
//#include "GameCloudSDKUE4/WebSocket/WebSocketBase.h"
// includes project files
#include "GameCloudSDKNetworkData.h"
// fixed final include
#include "ClientConnectionBlueprintLibrary.generated.h"

UENUM( BlueprintType )
enum class WebSocketDataType : uint8
{
	CharPtr			UMETA( DisplayName = "const char*" ),
	StdString		UMETA( DisplayName = "std::string" ),
	UnrealString	UMETA( DisplayName = "FString" )
};

/**
 * 
 */
UCLASS()
class GAMECLOUDSDKUE4_API UClientConnectionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static WebSocketDataType WSDataType;

protected:

public:
	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		static void EnableKeyboardMouse( bool Keyboard, bool Mouse );

	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		static void GetCoupon();

	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		static WebSocketDataType GetWSDataType();

	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		static void ShowGamepad( bool IsShowing );

	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		static void UpdateWSDataType( bool IsIncrement );

protected:

	//static FString GenerateString( GameCloud::GameCloudSDKNetworkMessageType type, bool n_args, ... );
};
