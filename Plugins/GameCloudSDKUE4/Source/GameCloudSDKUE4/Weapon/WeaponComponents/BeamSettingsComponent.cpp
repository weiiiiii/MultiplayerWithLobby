// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "BeamSettingsComponent.h"
// include STL
// includes Engine files
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
// include plugin files
// includes third party files
// includes project files

UBeamSettingsComponent::UBeamSettingsComponent()
	: Range( 0.0f )
	, CollisionChannel()
{

}
