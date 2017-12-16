// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "WebsocketWrapper.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4_Global.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UWebsocketWrapper::UWebsocketWrapper( const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get() */ )
	: Super( ObjectInitializer )
{
	//UCommonFunctions::PrintToLog( "UWebSocketInstance::UWebSocketInstance()" );
	//check( nullptr == _WebSocket_Instance );
	//UCommonFunctions::PrintToLog( "UWebSocketInstance::UWebSocketInstance()" );
	//_WebSocket_Instance	= this;
	if ( nullptr != _WebSocket_Client )
	{
		_WebSocket_Client->OnClosed.AddDynamic( this, &UWebsocketWrapper::WebSocketClosed );
		_WebSocket_Client->OnConnectComplete.AddDynamic( this, &UWebsocketWrapper::WebSocketConnected );
		_WebSocket_Client->OnConnectError.AddDynamic( this, &UWebsocketWrapper::WebSocketConnectError );
		_WebSocket_Client->OnReceiveData.AddDynamic( this, &UWebsocketWrapper::WebSocketReceive );
	}
	
}

void UWebsocketWrapper::WebSocketClosed()
{
	OnClosed.Broadcast();
}

void UWebsocketWrapper::WebSocketConnected()
{
	OnConnectComplete.Broadcast();
}

void UWebsocketWrapper::WebSocketConnectError( const FString& error )
{
	OnConnectError.Broadcast( error );
}

void UWebsocketWrapper::WebSocketReceive( const FString& message )
{
	OnReceiveData.Broadcast( message );
}

