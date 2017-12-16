// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


// include STL
// includes Engine files
// include plugin files
// includes third party files
#include "StaticJSON/staticjson.hpp"
#include "StaticJSON/unreal_types.h"
// includes project files
// fixed final include
#include "VoteData.generated.h"

/**
 * 
 */
UENUM( BlueprintType )
enum class VoteMessageType : uint8
{
	CREATE_VOTE					UMETA( DisplayName = "Create Vote" ),
	START_VOTE					UMETA( DisplayName = "Start Vote" ),
	STOP_VOTE					UMETA( DisplayName = "Stop Vote" ),
	VOTE						UMETA( DisplayName = "Vote" ),
	VOTE_RESULT					UMETA( DisplayName = "Vote Result" ),
	INVALID
};

//NOTE!! - Cannot be declared in a namespace
STATICJSON_DECLARE_ENUM( VoteMessageType,
{"CREATE_VOTE", VoteMessageType::CREATE_VOTE},
{"START_VOTE", VoteMessageType::START_VOTE},
{"STOP_VOTE", VoteMessageType::STOP_VOTE},
{"VOTE", VoteMessageType::VOTE},
{"VOTE_RESULT", VoteMessageType::VOTE_RESULT},
{"INVALID", VoteMessageType::INVALID} )

USTRUCT( BlueprintType )
struct FVoteMessageData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		VoteMessageType Type;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString Data;

	FVoteMessageData()
		: Type( VoteMessageType::INVALID )
		, Data()
	{

	}

	FVoteMessageData( VoteMessageType Type, FString Data )
		: Type( Type )
		, Data( Data )
	{

	}

	void staticjson_init( staticjson::ObjectHandler* h )
	{
		h->add_property( "Type", &Type );
		h->add_property( "Data", &Data );
		//h->set_flags( staticjson::Flags::DisallowUnknownKey );

		h->set_flags( staticjson::Flags::Default );
	}
};

USTRUCT( BlueprintType )
struct FVoteInfoData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString Name;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString Description;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FString ImagePath;

	FVoteInfoData()
		: Name( "" )
		, Description( "" )
		, ImagePath( "" )
	{

	}

	FVoteInfoData( FString Name, FString Description, FString ImagePath )
		: Name( Name )
		, Description( Description )
		, ImagePath( ImagePath )
	{

	}

	void staticjson_init( staticjson::ObjectHandler* h )
	{
		h->add_property( "Name", &Name );
		h->add_property( "Description", &Description );
		h->add_property( "ImagePath", &ImagePath );
		//h->set_flags( staticjson::Flags::DisallowUnknownKey );

		h->set_flags( staticjson::Flags::Default );
	}
};

USTRUCT( BlueprintType )
struct FVoteSessionData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TArray<FVoteInfoData> VoteData;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		int TimeInSeconds;

	FVoteSessionData()
		: VoteData()
		, TimeInSeconds( 0 )
	{

	}

	FVoteSessionData( TArray<FVoteInfoData> VoteData, int TimeInSeconds )
		: VoteData( VoteData )
		, TimeInSeconds( TimeInSeconds )
	{

	}

	void staticjson_init( staticjson::ObjectHandler* h )
	{
		h->add_property( "VoteData", &VoteData );
		h->add_property( "TimeInSeconds", &TimeInSeconds );
		//h->set_flags( staticjson::Flags::DisallowUnknownKey );

		h->set_flags( staticjson::Flags::Default );
	}
};

USTRUCT( BlueprintType )
struct FVoteResultData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		TMap<FString,int> Result;
	
	FVoteResultData()
		: Result()
	{

	}

	FVoteResultData( TMap<FString, int> Result )
		: Result( Result )
	{

	}

	void staticjson_init( staticjson::ObjectHandler* h )
	{
		h->add_property( "Result", &Result );
		//h->set_flags( staticjson::Flags::DisallowUnknownKey );

		h->set_flags( staticjson::Flags::Default );
	}
};
