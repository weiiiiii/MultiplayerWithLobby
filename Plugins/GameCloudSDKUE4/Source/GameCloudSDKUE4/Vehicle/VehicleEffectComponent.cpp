// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "VehicleEffectComponent.h"
// include STL
// includes Engine files
#include "Components/SkeletalMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "WheeledVehicleMovementComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"


// Sets default values for this component's properties
UVehicleEffectComponent::UVehicleEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UVehicleEffectComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	updateWheelEffectsComponent( DeltaTime );
	printVehicleStats();
}


void UVehicleEffectComponent::SetupWheelEffects()
{
	int spawnedCount = 0;
	for ( int i = 0; i < MovementComponent->Wheels.Num(); i++ )
	{
		spawnWheelEffectsComponent( i );
		spawnedCount++;
	}
}

// Called when the game starts
void UVehicleEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UVehicleEffectComponent::getVehicleSpeed() const
{
	return ( MovementComponent ) ? FMath::Abs( MovementComponent->GetForwardSpeed() ) : 0.0f;
}

void UVehicleEffectComponent::showMovementEffects( int wheelIndex, FMovementFXStruct movementFXStruct )
{
	// retrieve particle system component to modify
	UParticleSystemComponent* currentFXComponent = nullptr;
	if ( wheelParticles.size() > wheelIndex )
		currentFXComponent = wheelParticles[wheelIndex];
	UVehicleWheel* wheel = MovementComponent->Wheels[wheelIndex];
	UParticleSystem* movementFX = movementFXStruct.Particle;

	const float currentSpeed = getVehicleSpeed();
	// effects in WheelFX require wheels of the vehicle to be in contact
	// with the ground and moving to show the effects
	//if ( !isOnGround || 0.01f >= currentSpeed )

	bool enableFXComponent = false;
	if ( isOnGround )
	{
		//check brake input and handbrake
		if ( wheel->bAffectedByHandbrake )
		{
			enableFXComponent = true;
			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Green, "Handbrake on" );
		}
		else
		{
			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Handbrake off" );
			if ( movementFXStruct.SpawnParticlesOnMove )
			{
				UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Green, "Spawn particles on move : True" );
				if ( minimumSpeedToSpawnParticles < currentSpeed )
				{
					UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Green, "minimum speed to spawn reached" );
					if ( nullptr != movementFX )
					{
						enableFXComponent = true;
						UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Green, "Movement FX available" );
					}
					else
					{
						UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Movement FX NOT available" );
					}
				}
				else
				{
					UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "below minimum speed to spawn" );
				}
			}
			else
			{
				UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Spawn particles on move : False" );
			}
		}
	}

	if ( !enableFXComponent )
	{
		UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "DISABLE FX COMPONENT" );
		if ( nullptr != currentFXComponent )
			currentFXComponent->SetActive( false );
	}
	else
	{
		UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "ENABLE FX COMPONENT" );
		FString toPrint = "";

		if ( nullptr != currentFXComponent )
		{
			UParticleSystem* currentFX = currentFXComponent->Template;
			if ( currentFX != movementFX )
			{
				currentFXComponent->SetActive( false );
				currentFXComponent->bAutoDestroy = true;
				//currentFXComponent->DetachFromParent();
				currentFXComponent = nullptr;

				toPrint += "currentFX != movementFX";
			}
		}
		// create currentFXComponent if it is null
		if ( nullptr == currentFXComponent )
		{
			spawnWheelEffectsComponent( wheelIndex );
			currentFXComponent = wheelParticles[wheelIndex];

			if ( toPrint != "" )
				toPrint += " | ";
			toPrint += "spawnWheelEffectsComponent";
		}

		if ( currentFXComponent->Template != movementFX )
		{
			currentFXComponent->SetTemplate( movementFX );
			currentFXComponent->SetRelativeScale3D( movementFXStruct.Scale );

			if ( toPrint != "" )
				toPrint += " | ";
			toPrint += "set template";
		}
		// set template, activate and scale
		if ( !currentFXComponent->bHasBeenActivated
			|| !currentFXComponent->bIsActive
			|| currentFXComponent->bWasDeactivated
			|| currentFXComponent->bWasCompleted )
		{
			currentFXComponent->ActivateSystem();
			if ( toPrint != "" )
				toPrint += " | ";
			toPrint += "activate system!";
		}

		//float spawnRateOut = movementFXStruct.MaxSpawnRate;			
		//if ( currentSpeed < movementFXStruct.MaxSpawnRateSpeed )
		//{
		//	float fraction = currentSpeed / movementFXStruct.MaxSpawnRateSpeed;
		//	spawnRateOut *= fraction;
		//	if ( spawnRateOut >= 0.01f )					
		//		currentFXComponent->SetFloatParameter( "SpawnRate", movementFXStruct.MaxSpawnRate * fraction );
		//	else
		//		currentFXComponent->SetActive( false );
		//}
		//else
		//{
		//	currentFXComponent->SetFloatParameter( "SpawnRate", movementFXStruct.MaxSpawnRate );
		//}

		float spawnRate = movementFXStruct.SpawnRateCurve->GetFloatValue( currentSpeed );
		currentFXComponent->SetFloatParameter( "SpawnRate", spawnRate );
	}

	//if ( !isOnGround || minimumSpeedToSpawnParticles >= currentSpeed )
	//{
	//	if ( currentFXComponent )
	//		currentFXComponent->SetActive( false );
	//}
	//else
	//{
	//	//  disable fx component and continue
	//	UParticleSystem* movementFX = movementFXStruct.Particle;
	//	if ( nullptr == movementFX )
	//	{
	//		if ( currentFXComponent )
	//			currentFXComponent->SetActive( false );
	//	}
	//	else
	//	{
	//		FString toPrint = "";

	//		if ( nullptr != currentFXComponent )
	//		{
	//			UParticleSystem* currentFX = currentFXComponent->Template;
	//			if ( currentFX != movementFX )
	//			{
	//				currentFXComponent->SetActive( false );
	//				currentFXComponent->bAutoDestroy = true;
	//				//currentFXComponent->DetachFromParent();
	//				currentFXComponent = nullptr;

	//				toPrint += "currentFX != movementFX";
	//			}
	//		}
	//		// create currentFXComponent if it is null
	//		if ( nullptr == currentFXComponent )
	//		{
	//			spawnWheelEffectsComponent( wheelIndex );
	//			currentFXComponent = wheelParticles[wheelIndex];

	//			if ( toPrint != "" )
	//				toPrint += " | ";
	//			toPrint += "spawnWheelEffectsComponent";
	//		}

	//		if ( currentFXComponent->Template != movementFX )
	//		{
	//			currentFXComponent->SetTemplate( movementFX );
	//			currentFXComponent->SetRelativeScale3D( movementFXStruct.Scale );

	//			if ( toPrint != "" )
	//				toPrint += " | ";
	//			toPrint += "set template";
	//		}
	//		// set template, activate and scale
	//		if ( !currentFXComponent->bHasBeenActivated
	//			|| !currentFXComponent->bIsActive 
	//			|| currentFXComponent->bWasDeactivated 
	//			|| currentFXComponent->bWasCompleted )
	//		{
	//			currentFXComponent->ActivateSystem();
	//			if ( toPrint != "" )
	//				toPrint += " | ";
	//			toPrint += "activate system!";
	//		}

	//		//float spawnRateOut = movementFXStruct.MaxSpawnRate;			
	//		//if ( currentSpeed < movementFXStruct.MaxSpawnRateSpeed )
	//		//{
	//		//	float fraction = currentSpeed / movementFXStruct.MaxSpawnRateSpeed;
	//		//	spawnRateOut *= fraction;
	//		//	if ( spawnRateOut >= 0.01f )					
	//		//		currentFXComponent->SetFloatParameter( "SpawnRate", movementFXStruct.MaxSpawnRate * fraction );
	//		//	else
	//		//		currentFXComponent->SetActive( false );
	//		//}
	//		//else
	//		//{
	//		//	currentFXComponent->SetFloatParameter( "SpawnRate", movementFXStruct.MaxSpawnRate );
	//		//}

	//		float spawnRate = movementFXStruct.SpawnRateCurve->GetFloatValue( currentSpeed );
	//		currentFXComponent->SetFloatParameter( "SpawnRate", spawnRate );
	//	}
	//}
}

void UVehicleEffectComponent::spawnSkidEffects( int wheelIndex, FSkidFXStruct skidFXStruct )
{
	AActor* owner = GetOwner();
	check( owner );
	UWorld* world = owner->GetWorld();
	check( world );
	UVehicleWheel* wheel = MovementComponent->Wheels[wheelIndex];
	check( wheel );

	int skidKey = 20 + wheelIndex;
	UMaterial* skidDecalMaterial = skidFXStruct.Decal;
	if ( nullptr != skidDecalMaterial )
	{
		UMaterialInstanceDynamic* newMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance( world, Cast<UMaterialInterface>( skidDecalMaterial ) );
		if ( nullptr != newMat )
		{
			newMat->SetScalarParameterValue( FName( "Frame" ), UKismetMathLibrary::RandomFloatInRange( 0, 3.0f ) );
			FVector forwardVector = owner->GetActorForwardVector();
			FVector wheelForward = forwardVector.RotateAngleAxis( wheel->SteerAngle, FVector::UpVector );
			FRotator wheelRotation = wheelForward.Rotation();

			UDecalComponent* decalComponent = nullptr;
			decalComponent = UGameplayStatics::SpawnDecalAtLocation( world, newMat, skidFXStruct.DecalScale, wheel->Location, wheelRotation, skidFXStruct.DecalFadeDelay + skidFXStruct.DecalFadeOutDuration );
			if ( decalComponent )
			{
				decalComponent->SetFadeOut( skidFXStruct.DecalFadeDelay, skidFXStruct.DecalFadeOutDuration, false );
				//decalComponent->FadeScreenSize = ImpactDecalFadeScreenSize;

				//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "spawnSkidEffects : decal component created!", skidKey );
			}
			else
			{
				//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "spawnSkidEffects : decal component NOT created!", skidKey );
			}
		}
		else
		{
			//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "spawnSkidEffects : newMat created does not exist", skidKey );
		}
	}
	else
	{
		//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "spawnSkidEffects : skidFXStruct does not contain decal material", skidKey );
	}
}

void UVehicleEffectComponent::spawnWheelEffectsComponent( int wheelIndex )
{
	check( MovementComponent );
	AActor* owner = GetOwner();
	check( owner );
	UWorld* world = owner->GetWorld();
	check( world );
	USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>( owner->GetComponentByClass( USkeletalMeshComponent::StaticClass() ) );
	check( skeletalMeshComponent );
	check( wheelIndex < WheelParticlePlaceholders.Num() );

	UParticleSystemComponent* component = NewObject<UParticleSystemComponent>( owner );
	
	component->bAutoActivate = false;
	component->bAutoDestroy = false;
	component->RegisterComponentWithWorld( world );
	component->AttachToComponent( skeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform );
	// set relative location
	USceneComponent* placeholder = WheelParticlePlaceholders[wheelIndex];
	component->SetRelativeLocation( placeholder->GetRelativeTransform().GetLocation() );

	if ( wheelParticles.size() <= wheelIndex )
		wheelParticles.push_back( component );
	else
		wheelParticles[wheelIndex] = component;
}

void UVehicleEffectComponent::updateWheelEffectsComponent( float deltaTime )
{
	AActor* owner = GetOwner();
	check( owner );
	UWorld* world = owner->GetWorld();
	check( world );

	// play landing sound

	isOnGround = false;
	
	// variables for checking if vehicle should skid
	FVector velocity = owner->GetVelocity();
	bool hasVehicleStopped = velocity.SizeSquared2D() < SkidThresholdVelocity * SkidThresholdVelocity;
	bool isTireSlipping = MovementComponent->CheckSlipThreshold( LongSlipSkidThreshold, LateralSlipSkidThreshold );

	FString surfaceStr = "Surface in contact :   ";
	for ( int i = 0; i < MovementComponent->Wheels.Num(); i++ )
	{
		UVehicleWheel* wheel = MovementComponent->Wheels[i];
		UPhysicalMaterial* physMat = wheel->GetContactSurfaceMaterial();
		FWheelFXStruct fxStruct;

		if ( physMat != nullptr )
		{
			isOnGround = true;
			int index = (int)physMat->SurfaceType;
			checkf( WheelFX->Effects.Num() > index, *( "WheelFX does not support surface index " + FString::FromInt( index ) ) );
			fxStruct = WheelFX->Effects[index];
			checkf( nullptr != fxStruct.MovementFX.SpawnRateCurve, *FString( "Spawn rate curve is missing for surface " + FString::FromInt( index ) ) );

			surfaceStr += FString::FromInt( index ) + "   ";

			showMovementEffects( i, fxStruct.MovementFX );
			if ( isOnGround && !hasVehicleStopped && isTireSlipping )
				spawnSkidEffects( i, fxStruct.SkidFX );
		}
		else
		{
			surfaceStr += "#   ";
		}
	}

	UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Cyan, surfaceStr, 101 );
}

void UVehicleEffectComponent::printVehicleStats()
{
	const float currentSpeed = getVehicleSpeed();
	FString toPrint = "Speed[" + FString::SanitizeFloat( currentSpeed ) + "]";

	for ( int i = 0; i < wheelParticles.size(); i++ )
	{
		UParticleSystemComponent* currentFXComponent = wheelParticles[i];
		float spawnRate = 0;
		currentFXComponent->GetFloatParameter( "SpawnRate", spawnRate );
		toPrint += " [" + FString::SanitizeFloat( spawnRate ) + "]";
	}

	//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, toPrint, 100 );
}
