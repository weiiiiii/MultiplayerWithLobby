// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "EntityPlaceholderComponent.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files

UEntityPlaceholderComponent::UEntityPlaceholderComponent( const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get() */ )
	: SocketName()
	, TypeString()
{
}

void UEntityPlaceholderComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
