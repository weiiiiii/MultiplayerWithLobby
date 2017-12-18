// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "VoteWrapper.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4_Global.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/WebSocket/WebSocketBlueprintLibrary.h"

UVoteWrapper::UVoteWrapper( const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get() */ )
	: Super( ObjectInitializer )
{
//#if WITH_EDITOR == 0
	//UCommonFunctions::PrintToLog( "UWebSocketInstance::UWebSocketInstance()" );
	//check( nullptr == _WebSocket_Instance );
	//UCommonFunctions::PrintToLog( "UWebSocketInstance::UWebSocketInstance()" );
	//_WebSocket_Instance	= this;

	Websocket	= UWebSocketBlueprintLibrary::Connect( "ws://localhost:9001/playerroom1" );
	if ( nullptr != Websocket )
	{
		Websocket->OnClosed.AddDynamic( this, &UVoteWrapper::WebSocketClosed );
		Websocket->OnConnectComplete.AddDynamic( this, &UVoteWrapper::WebSocketConnected );
		Websocket->OnConnectError.AddDynamic( this, &UVoteWrapper::WebSocketConnectError );
		Websocket->OnReceiveData.AddDynamic( this, &UVoteWrapper::WebSocketReceive );
	}
//#else
//	UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Unable to use websocket in editor!" );
//	UCommonFunctions::PrintToLog( "Unable to send websocket in editor!" );
//#endif
}

void UVoteWrapper::StartVoteSession( FVoteSessionData VoteSessionData )
{
//#if WITH_EDITOR == 0
	if ( nullptr != Websocket )
	{
		{
			staticjson::ParseStatus status;
			std::string sessionDataStr = staticjson::to_json_string( VoteSessionData );
			FVoteMessageData messageData = FVoteMessageData( VoteMessageType::CREATE_VOTE, sessionDataStr.c_str() );
			std::string messageDataStr = staticjson::to_json_string( messageData );
			Websocket->SendRawData_StdString( messageDataStr );
		}
		
		{
			staticjson::ParseStatus status;
			FVoteMessageData messageData = FVoteMessageData( VoteMessageType::START_VOTE, "" );
			std::string messageDataStr = staticjson::to_json_string( messageData );
			Websocket->SendRawData_StdString( messageDataStr );
		}
	}
//#else
//	UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Unable to use websocket in editor!" );
//	UCommonFunctions::PrintToLog( "Unable to send websocket in editor!" );
//#endif
}

void UVoteWrapper::StopVoteSession()
{
//#if WITH_EDITOR == 0
	if ( nullptr != Websocket )
	{
		staticjson::ParseStatus status;
		FVoteMessageData messageData = FVoteMessageData( VoteMessageType::STOP_VOTE, "" );
		std::string messageDataStr = staticjson::to_json_string( messageData );
		Websocket->SendRawData_StdString( messageDataStr );
	}
//#else
//	UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Unable to use websocket in editor!" );
//	UCommonFunctions::PrintToLog( "Unable to send websocket in editor!" );
//#endif
}

void UVoteWrapper::ResetWebsocket()
{
	if ( nullptr != Websocket )
	{
		Websocket->OnClosed.RemoveDynamic( this, &UVoteWrapper::WebSocketClosed );
		Websocket->OnConnectComplete.RemoveDynamic( this, &UVoteWrapper::WebSocketConnected );
		Websocket->OnConnectError.RemoveDynamic( this, &UVoteWrapper::WebSocketConnectError );
		Websocket->OnReceiveData.RemoveDynamic( this, &UVoteWrapper::WebSocketReceive );
		Websocket->Close();
	}
	Websocket	= UWebSocketBlueprintLibrary::Connect( "ws://localhost:9001/playerroom1" );
	if ( nullptr != Websocket )
	{
		Websocket->OnClosed.AddDynamic( this, &UVoteWrapper::WebSocketClosed );
		Websocket->OnConnectComplete.AddDynamic( this, &UVoteWrapper::WebSocketConnected );
		Websocket->OnConnectError.AddDynamic( this, &UVoteWrapper::WebSocketConnectError );
		Websocket->OnReceiveData.AddDynamic( this, &UVoteWrapper::WebSocketReceive );
	}
}

void UVoteWrapper::WebSocketClosed()
{
	//UCommonFunctions::PrintStringOnScreen( 30.0f, FColor::Cyan, "Web socket closed!" );
	OnClosed.Broadcast();
	Websocket	= nullptr;
}

void UVoteWrapper::WebSocketConnected()
{
	//UCommonFunctions::PrintStringOnScreen( 30.0f, FColor::Yellow, "Web socket connected!" );
	OnConnectComplete.Broadcast();
}

void UVoteWrapper::WebSocketConnectError( const FString& error )
{
	//UCommonFunctions::PrintStringOnScreen( 30.0f, FColor::Green, "Web socket connect error!" );
	OnConnectError.Broadcast( error );
	Websocket	= nullptr;
}

void UVoteWrapper::WebSocketReceive( const FString& message )
{
	std::string messageStr = TCHAR_TO_UTF8( *message );
	OnReceiveData.Broadcast( message );
	//UCommonFunctions::PrintStringOnScreen( 30.0f, FColor::Red, messageStr );

	FVoteMessageData messageData;
	staticjson::ParseStatus status;
	staticjson::from_json_string( messageStr.c_str(), &messageData, &status );


	if ( VoteMessageType::VOTE_RESULT == messageData.Type )
	{
		std::string dataStr	= TCHAR_TO_UTF8( *messageData.Data );
		//UCommonFunctions::PrintStringOnScreen( 30.0f, FColor::Red, dataStr );
		FVoteResultData resultData;
		staticjson::from_json_string( dataStr.c_str(), &resultData, &status );
		OnReceiveResult.Broadcast( resultData );

		//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Red, "Result data has " + FString::SanitizeFloat( resultData.Result.Num() ) + " keys" );
	}
}

