// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// include STL
// includes Engine files
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Data/SQLiteDatabase.h"
// fixed final include
#include "GameCloudSDKUE4_GameInstance.generated.h"

//class USQLiteDatabase;

/**
*
*/
UCLASS()
class GAMECLOUDSDKUE4_API UGameCloudSDKUE4_GameInstance : public UGameInstance
{
	GENERATED_BODY()
 
public:
	UPROPERTY( BlueprintReadOnly )
		USQLiteDatabase* Database;

protected:

public:
	virtual void Init() override;

protected:
};
