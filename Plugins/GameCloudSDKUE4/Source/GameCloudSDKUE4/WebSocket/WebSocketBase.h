/*
* uewebsocket - unreal engine 4 websocket plugin
*
* Copyright (C) 2017 feiwu <feixuwu@outlook.com>
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation:
*  version 2.1 of the License.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*  MA  02110-1301  USA
*/

#pragma once

// include STL
// includes Engine files
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "UObject/NoExportTypes.h"
// include plugin files
// includes third party files
// includes project files
// fixed final include
#include "WebSocketBase.generated.h"

// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
// fixed final include

DEFINE_LOG_CATEGORY_STATIC( WebSocket, Log, All );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FWebSocketConnectError, const FString&, error );
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FWebSocketClosed );
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FWebSocketConnected );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FWebSocketRecieve, const FString&, data );

struct lws_context;
struct lws;

/**
*
*/
UCLASS( Blueprintable, BlueprintType )
class GAMECLOUDSDKUE4_API UWebSocketBase :public UObject
{
	GENERATED_BODY()
public:

	UWebSocketBase();

	virtual void BeginDestroy() override;

	void SendRawData_CharPtr( const char* data );
	void SendRawData_StdString( std::string data );

	UFUNCTION( BlueprintCallable, Category = WebSocket )
		void SendText( const FString& data );

	UFUNCTION( BlueprintCallable, Category = WebSocket )
		void Close();

	void Connect( const FString& uri, const TMap<FString, FString>& header );

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketConnectError OnConnectError;

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketClosed OnClosed;

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketConnected OnConnectComplete;

	UPROPERTY( BlueprintAssignable, Category = WebSocket )
		FWebSocketRecieve OnReceiveData;

	void Cleanlws();
	void ProcessWriteable();
	void ProcessWriteable_CharPtr();
	void ProcessWriteable_FString();
	void ProcessWriteable_StdString();
	void ProcessRead( const char* in, int len );
	bool ProcessHeader( unsigned char** p, unsigned char* end );

	struct lws_context* mlwsContext;
	struct lws* mlws;
	std::vector<const char*> mSendQueue_CharPtr;
	TArray<FString> mSendQueue_FString;
	std::vector<std::string> mSendQueue_StdString;
	TMap<FString, FString> mHeaderMap;
};
