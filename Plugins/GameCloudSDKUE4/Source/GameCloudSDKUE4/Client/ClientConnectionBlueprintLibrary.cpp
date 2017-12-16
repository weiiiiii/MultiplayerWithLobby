// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "ClientConnectionBlueprintLibrary.h"

//tofix put in stdafx.h
#include <sstream>
#include <bitset>

// include STL
#include<stdarg.h>
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4_Global.h"
#include "GameCloudSDKNetworkData.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
//#include "GameCloudSDKUE4/WebSocket/WebSocketBase.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/WebSocket/WebSocketBlueprintLibrary.h"

using namespace std;

WebSocketDataType UClientConnectionBlueprintLibrary::WSDataType	= WebSocketDataType::StdString;

namespace 
{
	void																	SendWebsocketMessage( const simple::mem_ostream <std::true_type> & out )
	{
#if WITH_EDITOR == 0
		if ( nullptr != _WebSocket_Client )
		{
			const char * buffer	= reinterpret_cast <const char*> ( out.get_internal_vec().data() );
			int bufferlength	= out.get_internal_vec().size();

			switch ( UClientConnectionBlueprintLibrary::WSDataType )
			{
				case WebSocketDataType::CharPtr:
					// send as const char *
				{
					// print the buffer
					std::ostringstream oss;
					oss << "[char*]   Length: " << bufferlength << " | Data: " << buffer << std::endl << "Bit: " << std::endl;
					for ( std::size_t i = 0; i < bufferlength; ++i )
					{ oss << "[" << i << "]" << std::bitset<8>( buffer[i] ) << std::endl; }
					UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, oss.str().c_str() );

					_WebSocket_Client->SendRawData_CharPtr( buffer );
				}
				break;

				case WebSocketDataType::StdString:
					// send as std::string
				{
					std::string message( buffer, bufferlength );

					// print the message
					std::ostringstream oss;
					oss << "[string]  Length: " << message.size() << " | Data: " << message << std::endl << "Bit: " << std::endl;
					for ( std::size_t i = 0; i < message.size(); ++i )
					{ oss << "[" << i << "]" << std::bitset<8>( message.c_str()[i] ) << std::endl; }
					UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Orange, oss.str().c_str() );

					_WebSocket_Client->SendRawData_StdString( message );
				}
				break;

				case WebSocketDataType::UnrealString:
					// send as FString
				{
					FString message	= FString( buffer );

					// print the message
					std::string fromFString	= TCHAR_TO_UTF8( *message );
					std::ostringstream oss;
					oss << "[string]  Length: " << message.Len() << " | Data: " << fromFString << std::endl << "Bit: " << std::endl;
					for ( std::size_t i = 0; i < fromFString.size(); ++i )
					{ oss << "[" << i << "]" << std::bitset<8>( fromFString.c_str()[i] ) << std::endl; }
					UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Yellow, oss.str().c_str() );

					_WebSocket_Client->SendText( message );
				}
				break;

				default:
					break;
			}
		}
		else
		{
			std::string toPrint	= "No websocket connection!";
			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, toPrint );
			UCommonFunctions::PrintToLog( FString( toPrint.c_str() ) );
		}
#else
		UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Unable to use websocket in editor!" );
		UCommonFunctions::PrintToLog( "Unable to send websocket in editor!" );
#endif
	}
}

void UClientConnectionBlueprintLibrary::EnableKeyboardMouse( bool Keyboard, bool Mouse )
{
	simple::mem_ostream <std::true_type> out;
	out << GameCloud::GameCloudSDKNetworkMessageType::EnableKeyboardMouse << Keyboard << Mouse;

	SendWebsocketMessage( out );
}

void UClientConnectionBlueprintLibrary::GetCoupon()
{
	simple::mem_ostream <std::true_type> out;
	out << GameCloud::GameCloudSDKNetworkMessageType::GetCoupon;

	SendWebsocketMessage( out );
}

WebSocketDataType UClientConnectionBlueprintLibrary::GetWSDataType()
{
	return WSDataType;
}

void UClientConnectionBlueprintLibrary::ShowGamepad( bool IsShowing )
{
	simple::mem_ostream <std::true_type> out;
	out << GameCloud::GameCloudSDKNetworkMessageType::ShowGamePad << IsShowing;

	SendWebsocketMessage ( out );
}

void UClientConnectionBlueprintLibrary::UpdateWSDataType( bool IsIncrement )
{
	int index	= (int)WSDataType + ( IsIncrement ? 1 : -1 );
	if ( index < 0 )
	{ index = 0;  }
	else if ( index >( int )WebSocketDataType::UnrealString )
	{ index = (int)WebSocketDataType::UnrealString; }

	WSDataType	= WebSocketDataType( index );
}

//FString UClientConnectionBlueprintLibrary::GenerateString( GameCloud::GameCloudSDKNetworkMessageType type, bool n_args, ... )
//{
//	simple::mem_ostream <std::true_type>  out;
//	out << type;
//
//	va_list ap;
//	va_start( ap, n_args );
//
//	bool flag = va_arg( ap, bool );
//	while ( flag != NULL )
//	{
//		out << flag;
//		flag = va_arg( ap, bool );
//	}
//	va_end( ap );
//
//	const char * buffer = reinterpret_cast <const char*> ( out.get_internal_vec().data() );
//	FString toReturn	= FString( buffer );
//	return toReturn;
//}
