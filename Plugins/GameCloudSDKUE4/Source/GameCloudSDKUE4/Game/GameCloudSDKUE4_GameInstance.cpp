// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "GameCloudSDKUE4_GameInstance.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files

void UGameCloudSDKUE4_GameInstance::Init()
{
	Database	= NewObject<USQLiteDatabase>( this, "GameDatabase" );
	Super::Init();	
}
