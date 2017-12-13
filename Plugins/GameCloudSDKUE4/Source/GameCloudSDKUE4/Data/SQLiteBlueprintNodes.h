
#pragma once

// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
//#include "StaticJSON/staticjson.hpp"
//#include "StaticJSON/unreal_types.h"
// fixed final include
#include "SQLiteBlueprintNodes.generated.h"

USTRUCT(BlueprintType)
struct GAMECLOUDSDKUE4_API FSQLiteQueryLogicExpectedNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Database Query")
	FString Query;

	FSQLiteQueryLogicExpectedNode(){}
	FSQLiteQueryLogicExpectedNode(FString LHSQuery, FString Append) : Query(LHSQuery) 
	{
		Query += Append;
	}
};

USTRUCT(BlueprintType)
struct GAMECLOUDSDKUE4_API FSQLiteQueryTermExpectedNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Database Query")
	FString Query;

	FSQLiteQueryTermExpectedNode(){}
	FSQLiteQueryTermExpectedNode(FString LHSQuery, FString Append) : Query(LHSQuery) 
	{
		Query += Append;
	}
};

USTRUCT(BlueprintType)
struct GAMECLOUDSDKUE4_API FSQLiteQueryFinalizedQuery
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLite Database Query")
	FString Query;

	FSQLiteQueryFinalizedQuery(){}
	FSQLiteQueryFinalizedQuery(FString Query) : Query(Query){}
};
