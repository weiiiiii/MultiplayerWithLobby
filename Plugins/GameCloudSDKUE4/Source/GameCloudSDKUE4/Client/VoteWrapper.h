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
#include "GameCloudSDKUE4/Client/VoteData.h"
#include "GameCloudSDKUE4/WebSocket/WebSocketBase.h"
// fixed final include
#include "VoteWrapper.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FVoteResultReceive, const FVoteResultData&, VoteResultData );

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, ClassGroup = ( GameCloudSDKUE4 ) )
class GAMECLOUDSDKUE4_API UVoteWrapper :public UObject
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

	UPROPERTY( BlueprintAssignable, Category = Voting )
		FVoteResultReceive OnReceiveResult;

	UWebSocketBase* Websocket;
public:
	UVoteWrapper( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );

	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		void StartVoteSession( FVoteSessionData VoteSessionData );
	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		void StopVoteSession();
	UFUNCTION( BlueprintCallable, Category = "Client Connection" )
		void ResetWebsocket();
	UFUNCTION()
		void WebSocketClosed();
	UFUNCTION()
		void WebSocketConnected();
	UFUNCTION()
		void WebSocketConnectError( const FString& error );
	UFUNCTION()
		void WebSocketReceive( const FString& message );
};
