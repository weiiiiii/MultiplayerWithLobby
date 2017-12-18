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
#include "WebSocketContext.h"
// include STL
// includes Engine files
#include "UObjectGlobals.h"
// include plugin files
// includes third party files
// includes project files
#include "WebSocketBase.h"


#define MAX_PAYLOAD	64*1024

extern TSharedPtr<UWebSocketContext> s_websocketCtx;

static struct lws_protocols protocols[] ={
	/* first protocol must always be HTTP handler */

	{
		"",		/* name - can be overridden with -e */
		UWebSocketContext::callback_echo,
		0,
		MAX_PAYLOAD,
	},
	{
		NULL, NULL, 0		/* End of list */
	}
};

static const struct lws_extension exts[] ={
	{
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate; client_no_context_takeover"
	},
	{
		"deflate-frame",
		lws_extension_callback_pm_deflate,
		"deflate_frame"
	},
	{NULL, NULL, NULL /* terminator */}
};

void UWebSocketContext::BeginDestroy()
{
	Super::BeginDestroy();
	s_websocketCtx.Reset();
}

#include "GameCloudSDKUE4/Common/CommonFunctions.h"
int UWebSocketContext::callback_echo( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : Message Length[" + FString::FromInt( len ) + "]" );
	void* pUser = lws_wsi_user( wsi );
	UWebSocketBase* pWebSocketBase = (UWebSocketBase*)pUser;

	switch ( reason )
	{
		case LWS_CALLBACK_CLOSED:
			//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : LWS_CALLBACK_CLOSED" );
			if ( !pWebSocketBase ) return -1;
			pWebSocketBase->Cleanlws();
			pWebSocketBase->OnClosed.Broadcast();
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		{
			if ( !pWebSocketBase ) return -1;
			//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : LWS_CALLBACK_CLIENT_CONNECTION_ERROR" );
			FString strError = UTF8_TO_TCHAR( in );
			UE_LOG( WebSocket, Error, TEXT( "libwebsocket connect error:%s" ), *strError );
			pWebSocketBase->Cleanlws();
			pWebSocketBase->OnConnectError.Broadcast( strError );
		}
		break;

		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : LWS_CALLBACK_CLIENT_ESTABLISHED" );
			if ( !pWebSocketBase ) return -1;
			pWebSocketBase->OnConnectComplete.Broadcast();
			break;

		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
		{
			//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER" );
			if ( !pWebSocketBase ) return -1;

			unsigned char **p = (unsigned char **)in, *end = ( *p ) + len;
			if ( !pWebSocketBase->ProcessHeader( p, end ) )
			{
				return -1;
			}
		}
		break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
		{
			//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : LWS_CALLBACK_CLIENT_RECEIVE" );
			if ( !pWebSocketBase )
			{
				//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Red, "Websocket is INVALID!" );
				return -1;
			}
			FString inStr = FString( (const char*)in );
			//UCommonFunctions::PrintFStringOnScreen( 30.0f, FColor::Orange, "WebSocketBase::ProcessRead : " + inStr + " Len : " + FString::FromInt( len ) );
			pWebSocketBase->ProcessRead( (const char*)in, (int)len );
		}	break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
			//UCommonFunctions::PrintFStringOnScreen( 10.0f, FColor::Orange, "UWebSocketContext::callback_echo : LWS_CALLBACK_CLIENT_WRITEABLE" );
			if ( !pWebSocketBase ) return -1;
			pWebSocketBase->ProcessWriteable();
			break;

		default:
			break;
	}

	return 0;
}

UWebSocketContext::UWebSocketContext()
{
	mlwsContext = nullptr;
}

void UWebSocketContext::CreateCtx()
{
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof info );

	info.protocols = protocols;
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;

	info.port = -1;
	info.gid = -1;
	info.uid = -1;
	info.extensions = exts;
	info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;
	info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

	mlwsContext = lws_create_context( &info );
	if ( mlwsContext == nullptr )
	{
		//UE_LOG(WebSocket, Error, TEXT("libwebsocket Init fail"));
	}
}

void UWebSocketContext::Tick( float DeltaTime )
{
	if ( mlwsContext != nullptr )
	{
		lws_callback_on_writable_all_protocol( mlwsContext, &protocols[0] );
		lws_service( mlwsContext, 0 );
	}
}

bool UWebSocketContext::IsTickable() const
{
	return true;
}

TStatId UWebSocketContext::GetStatId() const
{
	return TStatId();
}

UWebSocketBase* UWebSocketContext::Connect( const FString& uri, const TMap<FString, FString>& header )
{
	UE_LOG( WebSocket, Log, TEXT( "UWebSocketContext::Connect : :%s" ), *uri );
	if ( mlwsContext == nullptr )
	{
		return nullptr;
	}

	UWebSocketBase* pNewSocketBase = NewObject<UWebSocketBase>();
	pNewSocketBase->mlwsContext = mlwsContext;

	pNewSocketBase->Connect( uri, header );
	UE_LOG( WebSocket, Log, TEXT( "UWebSocketContext::Connect : :%s" ), *uri );

	return pNewSocketBase;
}

UWebSocketBase* UWebSocketContext::Connect( const FString& uri )
{
	return Connect( uri, TMap<FString, FString>() );
}
