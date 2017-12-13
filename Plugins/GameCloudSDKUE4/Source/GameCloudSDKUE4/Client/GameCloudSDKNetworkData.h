#pragma once

namespace GameCloud
{
	// Note, this needs to be synchronized with client/server
	enum GameCloudSDKNetworkMessageType : unsigned char
	{
/// tofix this does not work with enum value being 0 (issue with binstream)
// 		  GetCoupon
// 		, ShowGamePad
// 		, EnableKeyboardMouse

		 GetCoupon           = 1
		,ShowGamePad         = 2
		,EnableKeyboardMouse = 3

		,END
	};
}
