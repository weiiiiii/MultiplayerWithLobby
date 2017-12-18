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

// fixed first include
#include "WebSocketBase.h"
// include STL
#include <iostream>
// includes Engine files
// include plugin files
// includes third party files
#include "ThirdParty/WebSocket/include/Win64/libwebsockets.h"
//#include "libwebsockets.h"
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"


// fixed first include
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files

#define MAX_ECHO_PAYLOAD 64*1024

UWebSocketBase::UWebSocketBase()
{
	mlwsContext = nullptr;
	mlws = nullptr;
}


void UWebSocketBase::BeginDestroy()
{
	Super::BeginDestroy();

	if ( mlws != nullptr )
	{
		lws_set_wsi_user( mlws, NULL );
		mlws = nullptr;
	}
}

void UWebSocketBase::Connect( const FString& uri, const TMap<FString, FString>& header )
{
	if ( uri.IsEmpty() )
	{
		return;
	}

	if ( mlwsContext == nullptr )
	{
		return;
	}

	int iUseSSL = 0;
	int iPos = uri.Find( TEXT( ":" ) );
	if ( iPos == INDEX_NONE )
	{
		//UE_LOG(WebSocket, Error, TEXT("Invalid Websocket address:%s"), *uri);
		return;
	}

	FString strProtocol = uri.Left( iPos );
	if ( strProtocol.ToUpper() != TEXT( "WS" ) && strProtocol.ToUpper() != TEXT( "WSS" ) )
	{
		//UE_LOG(WebSocket, Error, TEXT("Invalid Protol:%s"), *strProtocol);
		return;
	}

	if ( strProtocol.ToUpper() == TEXT( "WSS" ) )
	{
		iUseSSL = 2;
	}

	FString strHost;
	FString strPath = TEXT( "/" );
	FString strNextParse = uri.Mid( iPos + 3 );
	iPos = strNextParse.Find( "/" );
	if ( iPos != INDEX_NONE )
	{
		strHost = strNextParse.Left( iPos );
		strPath = strNextParse.Mid( iPos );
	}
	else
	{
		strHost = strNextParse;
	}

	FString strAddress = strHost;
	int iPort = 80;
	iPos = strAddress.Find( ":" );
	if ( iPos != INDEX_NONE )
	{
		strAddress = strHost.Left( iPos );
		iPort = FCString::Atoi( *strHost.Mid( iPos + 1 ) );
	}
	else
	{
		if ( iUseSSL )
		{
			iPort = 443;
		}
	}

	struct lws_client_connect_info connectInfo;
	memset( &connectInfo, 0, sizeof( connectInfo ) );

	std::string stdAddress = TCHAR_TO_UTF8( *strAddress );
	std::string stdPath = TCHAR_TO_UTF8( *strPath );
	std::string stdHost = TCHAR_TO_UTF8( *strHost );;

	connectInfo.context = mlwsContext;
	connectInfo.address = stdAddress.c_str();
	connectInfo.port = iPort;
	connectInfo.ssl_connection = iUseSSL;
	connectInfo.path = stdPath.c_str();
	connectInfo.host = stdHost.c_str();
	connectInfo.origin = stdHost.c_str();
	connectInfo.ietf_version_or_minus_one = -1;
	connectInfo.userdata = this;

	mlws = lws_client_connect_via_info( &connectInfo );
	//mlws = lws_client_connect_extended(mlwsContext, TCHAR_TO_UTF8(*strAddress), iPort, iUseSSL, TCHAR_TO_UTF8(*strPath), TCHAR_TO_UTF8(*strHost), TCHAR_TO_UTF8(*strHost), NULL, -1, (void*)this);
	if ( mlws == nullptr )
	{
		UE_LOG( WebSocket, Error, TEXT( "create client connect fail" ) );
		return;
	}

	mHeaderMap = header;
}

// WARNING : Doesn't work! Nothing is received by websocket server.
void UWebSocketBase::SendRawData_CharPtr( const char* data )
{
	if ( sizeof( data ) > MAX_ECHO_PAYLOAD )
	{
		UE_LOG( WebSocket, Error, TEXT( "too large package to send > MAX_ECHO_PAYLOAD:%d > %d" ), sizeof( data ), MAX_ECHO_PAYLOAD );
		return;
	}

	if ( mlws != nullptr )
	{
		mSendQueue_CharPtr.push_back( data );
	}
	else
	{
		UE_LOG( WebSocket, Error, TEXT( "the socket is closed, SendText fail" ) );
	}
}

// NOTE : The only function that works with websocket server!
void UWebSocketBase::SendRawData_StdString( std::string data )
{
	if ( sizeof( data ) > MAX_ECHO_PAYLOAD )
	{
		UE_LOG( WebSocket, Error, TEXT( "too large package to send > MAX_ECHO_PAYLOAD:%d > %d" ), sizeof( data ), MAX_ECHO_PAYLOAD );
		return;
	}

	if ( mlws != nullptr )
	{
		mSendQueue_StdString.push_back( data );
	}
	else
	{
		UE_LOG( WebSocket, Error, TEXT( "the socket is closed, SendText fail" ) );
	}
}

// WARNING : Doesn't work! Rubbish received by websocket server.
void UWebSocketBase::SendText( const FString& data )
{
	if ( data.Len() > MAX_ECHO_PAYLOAD )
	{
		UE_LOG( WebSocket, Error, TEXT( "too large package to send > MAX_ECHO_PAYLOAD:%d > %d" ), data.Len(), MAX_ECHO_PAYLOAD );
		return;
	}

	if ( mlws != nullptr )
	{
		mSendQueue_FString.Add( data );
	}
	else
	{
		UE_LOG( WebSocket, Error, TEXT( "the socket is closed, SendText fail" ) );
	}
}


void UWebSocketBase::ProcessWriteable()
{
	ProcessWriteable_CharPtr();
	ProcessWriteable_FString();
	ProcessWriteable_StdString();
}

void UWebSocketBase::ProcessWriteable_CharPtr()
{
	while ( mSendQueue_CharPtr.size() > 0 )
	{
		const char* data = mSendQueue_CharPtr[0];

		unsigned char buf[LWS_PRE + MAX_ECHO_PAYLOAD];
		memcpy( &buf[LWS_PRE], data, sizeof( data ) );
		//lws_write( mlws, &buf[LWS_PRE], sizeof( data ), LWS_WRITE_TEXT );
		lws_write( mlws, &buf[LWS_PRE], sizeof( data ), LWS_WRITE_BINARY );

		mSendQueue_CharPtr.erase( mSendQueue_CharPtr.begin() );
	}
}

void UWebSocketBase::ProcessWriteable_FString()
{
	while ( mSendQueue_FString.Num() > 0 )
	{
		std::string strData = TCHAR_TO_UTF8( *mSendQueue_FString[0] );

		unsigned char buf[LWS_PRE + MAX_ECHO_PAYLOAD];
		memcpy( &buf[LWS_PRE], strData.c_str(), strData.size() );
		//lws_write( mlws, &buf[LWS_PRE], strData.size(), LWS_WRITE_TEXT );
		lws_write( mlws, &buf[LWS_PRE], strData.size(), LWS_WRITE_BINARY );

		mSendQueue_FString.RemoveAt( 0 );
	}
}

void UWebSocketBase::ProcessWriteable_StdString()
{
	while ( mSendQueue_StdString.size() > 0 )
	{
		std::string strData = mSendQueue_StdString[0];
		
		unsigned char buf[LWS_PRE + MAX_ECHO_PAYLOAD];
		memcpy( &buf[LWS_PRE], strData.c_str(), strData.size() );
		//lws_write( mlws, &buf[LWS_PRE], strData.size(), LWS_WRITE_TEXT );
		lws_write( mlws, &buf[LWS_PRE], strData.size(), LWS_WRITE_BINARY );

		mSendQueue_StdString.erase( mSendQueue_StdString.begin() );
	}
}

void UWebSocketBase::ProcessRead( const char* in, int len )
{
	FString strData = UTF8_TO_TCHAR( in );
	//UCommonFunctions::PrintFStringOnScreen( 30.0f, FColor::Orange, "WebSocketBase::ProcessRead : " + strData );
	//UCommonFunctions::PrintFStringOnScreen( 30.0f, FColor::Orange, FString::FromInt( OnReceiveData.GetAllObjects().Num() ) );
	

	OnReceiveData.Broadcast( strData );
}


bool UWebSocketBase::ProcessHeader( unsigned char** p, unsigned char* end )
{
	if ( mHeaderMap.Num() == 0 )
	{
		return true;
	}

	for ( auto& it : mHeaderMap )
	{
		std::string strKey = TCHAR_TO_UTF8( *( it.Key ) );
		std::string strValue = TCHAR_TO_UTF8( *( it.Value ) );

		strKey += ":";
		if ( lws_add_http_header_by_name( mlws, (const unsigned char*)strKey.c_str(), (const unsigned char*)strValue.c_str(), (int)strValue.size(), p, end ) )
		{
			return false;
		}
	}

	return true;
}

void UWebSocketBase::Close()
{
	if ( mlws != nullptr )
	{
		lws_set_wsi_user( mlws, NULL );
		mlws = nullptr;
	}

	OnClosed.Broadcast();
}

void UWebSocketBase::Cleanlws()
{
	if ( mlws != nullptr )
	{
		lws_set_wsi_user( mlws, NULL );
	}
}
