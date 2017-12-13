#pragma once

#ifdef GAME_CLOUD_SDK_EXPORTS
#define GAME_CLOUD_SDK_API __declspec(dllexport)
#else
#define GAME_CLOUD_SDK_API __declspec(dllimport)
#endif // GAME_CLOUD_SDK_EXPORTS

class GAME_CLOUD_SDK_API GameCloudSDK
{
public:
	GameCloudSDK();
	~GameCloudSDK();

	static const char* SayHi();
};

extern GAME_CLOUD_SDK_API int SquareThisInt( int value );