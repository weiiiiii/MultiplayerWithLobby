// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "CreateOnBeginPlay.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files

UCreateOnBeginPlay::UCreateOnBeginPlay()
{
}

void UCreateOnBeginPlay::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner = GetOwner();
	if ( owner )
		for ( auto spawn : SpawnOnEvent )
			spawn.SpawnObjects( owner );
}
