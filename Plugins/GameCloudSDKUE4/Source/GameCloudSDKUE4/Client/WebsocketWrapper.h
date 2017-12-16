// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "UObject/NoExportTypes.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/WebSocket/WebSocketBase.h"
// fixed final include
#include "WebsocketWrapper.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FWebSocketConnectError, const FString&, error );
//DECLARE_DYNAMIC_MULTICAST_DELEGATE( FWebSocketClosed );
//DECLARE_DYNAMIC_MULTICAST_DELEGATE( FWebSocketConnected );
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FWebSocketRecieve, const FString&, data );

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, ClassGroup = ( GameCloudSDKUE4 ) )
class GAMECLOUDSDKUE4_API UWebsocketWrapper :public UObject
{
	GENERATED_BODY()

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketClosed OnClosed;

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketConnected OnConnectComplete;

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketConnectError OnConnectError;

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketRecieve OnReceiveData;

public:
	UWebsocketWrapper( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );
	
	UFUNCTION()
		void WebSocketClosed();
	UFUNCTION()
		void WebSocketConnected();
	UFUNCTION()
		void WebSocketConnectError( const FString& error );
	UFUNCTION()
		void WebSocketReceive( const FString& message );
};
