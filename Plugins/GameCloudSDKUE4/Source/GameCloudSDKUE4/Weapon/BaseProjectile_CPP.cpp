// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "BaseProjectile_CPP.h"
// include STL
// includes Engine files
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

ABaseProjectile_CPP::ABaseProjectile_CPP()
{
}

void ABaseProjectile_CPP::BeginPlay()
{
	Super::BeginPlay();
	setInstigatorFromParent();
}

void ABaseProjectile_CPP::setInstigatorFromParent()
{
	if ( nullptr != Instigator )
		return;

	AActor* owner = GetParentActor();
	if ( nullptr == owner )
		return;

	Instigator = owner->Instigator;
	return;
}
