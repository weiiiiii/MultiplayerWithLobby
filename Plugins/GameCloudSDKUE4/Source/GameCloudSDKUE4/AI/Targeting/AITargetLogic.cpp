// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "AITargetLogic.h"
// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/AI/Targeting/AISensorBase.h"
#include "GameCloudSDKUE4/Weapon/WeaponComponents/WeaponLogic.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UAITargetLogic::UAITargetLogic()
	: SortType( SortTargetBy::Closest )
	, TargetLogic( TargetingLogic::Till_Death )
	, RotationMinumum( FRotator::ZeroRotator )
	, RotationMaximum( FRotator::ZeroRotator )
	, AngularVelocity( FRotator::ZeroRotator )
	, SenseCooldown( 1.0f )
	, ShouldRotateToStartRotation( true )

	, SensorBase( nullptr )
	, RollComponent( nullptr )
	, PitchComponent( nullptr )
	, YawComponent( nullptr )
	, WeaponComponent( nullptr )
	, WeaponLogic( nullptr )
	, AttackSpawnComponent( nullptr )
	//, AttackSpawnComponents()
	//, Targets_()
	, Target( nullptr )

	, Cooldown_( 0.0f )
	, StartingRotators()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAITargetLogic::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if ( nullptr != Target )
	{
		// reset Target_ if it is out of sensor range
		if ( ! SensorBase->IsTargetInRange( Target ) )
		{
			UEntityComponent* entityComponent	= Cast<UEntityComponent>( Target->GetComponentByClass( UEntityComponent::StaticClass() ) );
			entityComponent->OnEntityKill.RemoveDynamic( this, &UAITargetLogic::OnTargetDeath );
			entityComponent->OnEntityDestroy.RemoveDynamic( this, &UAITargetLogic::OnTargetDestroyed );
			Target = nullptr;
		}
		else
		{
			// reset Target_ if it is out of weapon / projectile range
		}
	}

	if ( 0 < Cooldown_ )
	{ Cooldown_ -= DeltaTime; }
	else
	{
		if ( nullptr == Target )
		{
			FindTarget();
			Cooldown_	= SenseCooldown;
		}
	}

	Rotate( DeltaTime );
}

bool UAITargetLogic::IsTargetInShootingRange()
{
	bool result = false;
	if ( nullptr != Target )
	{
		AActor* owner	= GetOwner();
		check( owner );

		// get weapon's forward vector
		FVector weaponForwardVector		= WeaponComponent->GetForwardVector();
		FVector targetLocation			= Target->GetActorLocation();
		FVector targetDirection			= targetLocation - WeaponComponent->GetComponentLocation();
		FRotator relativeRotation		= UKismetMathLibrary::MakeRotationFromAxes( targetDirection, FVector::ZeroVector, FVector::ZeroVector );
		FRotator forwardRotation		= UKismetMathLibrary::MakeRotationFromAxes( weaponForwardVector, FVector::ZeroVector, FVector::ZeroVector );
		FRotator newRelativeRotation	= relativeRotation - forwardRotation;

		if ( fabs( newRelativeRotation.Yaw ) <= ShootAngleYaw )
		{
			if ( fabs( newRelativeRotation.Pitch ) <= ShootAnglePitch )
			{ result = true; }
		}
	}
	return result;
}

void UAITargetLogic::BeginPlay()
{
	Super::BeginPlay();
	
	std::vector<USceneComponent*> componentsToRotate	=
	{
		RollComponent,
		PitchComponent,
		YawComponent
	};
	for ( int i = 0; i < componentsToRotate.size(); i++ )
	{
		USceneComponent* componentToRotate = componentsToRotate[i];
		SetStartingRotator( componentToRotate );
	}
	if ( 0 >= componentsToRotate.size() )
	{ SetComponentTickEnabled( false ); }
}

void UAITargetLogic::FindTarget()
{
	AActor* owner	= GetOwner();
	check( owner );
	
	if ( nullptr != SensorBase )
	{
		TArray<AActor*> targets = SensorBase->GetTargets();

		if ( 0 < targets.Num() )
		{
			switch ( SortType )
			{
				case SortTargetBy::Closest:
					targets.StableSort( [this, owner]( AActor& A, AActor& B )
					{
						float distA		= FVector::Distance( ( A ).GetActorLocation(), owner->GetActorLocation() );
						float distB		= FVector::Distance( ( B ).GetActorLocation(), owner->GetActorLocation() );
						return distA < distB;
					} );
					break;
				case SortTargetBy::Furthest:
					targets.StableSort( [this, owner]( AActor& A, AActor& B )
					{
						float distA		= FVector::Distance( ( A ).GetActorLocation(), owner->GetActorLocation() );
						float distB		= FVector::Distance( ( B ).GetActorLocation(), owner->GetActorLocation() );
						return distA > distB;
					} );
					break;
				case SortTargetBy::Highest_HP:
					break;
				case SortTargetBy::Lowest_HP:
					break;

				default:
					break;
			}

			Target	= targets[0];
			UEntityComponent* entityComponent	= Cast<UEntityComponent>( Target->GetComponentByClass( UEntityComponent::StaticClass() ) );
			if ( !entityComponent->OnEntityKill.IsAlreadyBound( this, &UAITargetLogic::OnTargetDeath ) )
			{ entityComponent->OnEntityKill.AddDynamic( this, &UAITargetLogic::OnTargetDeath ); }
			if ( !entityComponent->OnEntityDestroy.IsAlreadyBound( this, &UAITargetLogic::OnTargetDestroyed ) )
			{ entityComponent->OnEntityDestroy.AddDynamic( this, &UAITargetLogic::OnTargetDestroyed); }
		}
	}
}

void UAITargetLogic::Rotate( float DeltaTime )
{
	std::vector<USceneComponent*> componentsToRotate =
	{
		RollComponent,
		PitchComponent,
		YawComponent
	};

	for ( int i = 0; i < componentsToRotate.size(); i++ )
	{
		USceneComponent* componentToRotate	= componentsToRotate[i];
		if ( nullptr != componentToRotate )
		{
			RotationType rotationType		= RotationType( i );
			FRotator currentRotation		= componentToRotate->RelativeRotation;// componentToRotate->GetComponentRotation();
			if ( nullptr == Target )
			{
				if ( ShouldRotateToStartRotation )
				{
					FRotator startingRotation		= StartingRotators[i];
					//RotateComponent( startingRotation, currentRotation, componentToRotate, DeltaTime, rotationType );
					FRotator diffInRotation			= startingRotation - currentRotation;
					RotateComponent2( diffInRotation, componentToRotate, DeltaTime, rotationType );
				}
			}
			else
			{
				UEntityComponent* entityComponent	= Cast<UEntityComponent>( Target->GetComponentByClass( UEntityComponent::StaticClass() ) );
				FVector targetLocation				= entityComponent->GetComponentLocation();
				FVector actorLocation				= WeaponComponent->GetComponentLocation();
				FVector directionToFace				= targetLocation - actorLocation;
				FRotator targetRotation				= UKismetMathLibrary::MakeRotationFromAxes( directionToFace, FVector::ZeroVector, FVector::ZeroVector );
				//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "Target wor Rotation : " + targetRotation.ToCompactString(), 1000 );
				FRotator compWorldRotation			= WeaponComponent->GetComponentRotation();

				//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "Comp World Rotation : " + compWorldRotation.ToCompactString(), 1001 );
				//UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, "Comp Relat Rotation : " + currentRotation.ToCompactString(), 1002 );
				FRotator diffInRotation				= targetRotation - WeaponComponent->GetComponentRotation();
				//targetRotation				-= componentToRotate->GetComponentRotation();
				//RotateComponent( targetRotation, currentRotation, componentToRotate, DeltaTime, rotationType );
				RotateComponent2( diffInRotation, componentToRotate, DeltaTime, rotationType );
			}
		}		
	}
}

void UAITargetLogic::RotateComponent( FRotator TargetRotation, FRotator CurrentRotation, USceneComponent* Component, float DeltaTime, RotationType Type )
{
	float TargetAngle			= 0.0f;
	float CurrentAngle			= 0.0f;
	float RotationMinimumToUse	= 0.0f;
	float RotationMaximumToUse	= 0.0f;
	float AngluarSpeed			= 0.0f;

	switch ( Type )
	{
		case RotationType::Roll:
			TargetAngle				= TargetRotation.Roll;
			CurrentAngle			= CurrentRotation.Roll;
			RotationMinimumToUse	= RotationMinumum.Roll;
			RotationMaximumToUse	= RotationMaximum.Roll;
			AngluarSpeed			= AngularVelocity.Roll;
			break;
		case RotationType::Pitch:
			TargetAngle				= TargetRotation.Pitch;
			CurrentAngle			= CurrentRotation.Pitch;
			RotationMinimumToUse	= RotationMinumum.Pitch;
			RotationMaximumToUse	= RotationMaximum.Pitch;
			AngluarSpeed			= AngularVelocity.Pitch;
			break;
		case RotationType::Yaw:
			TargetAngle				= TargetRotation.Yaw;
			CurrentAngle			= CurrentRotation.Yaw;
			RotationMinimumToUse	= RotationMinumum.Yaw;
			RotationMaximumToUse	= RotationMaximum.Yaw;
			AngluarSpeed			= AngularVelocity.Yaw;
			break;
	}

	if ( TargetAngle > RotationMinimumToUse )
	{
		if ( TargetAngle < RotationMaximumToUse )
		{
			float deltaAngle		= DeltaTime * AngluarSpeed;
			float modTargetAngle	= TargetAngle - CurrentAngle;
			if ( modTargetAngle > 180.0f )
			{ modTargetAngle		-= 360.0f; }
			else if ( modTargetAngle < -180.0f )
			{ modTargetAngle		+= 360.0f; }
			if ( modTargetAngle < 0.0f )
			{ deltaAngle			*= -1; }

			//float newAngle			= CurrentAngle + deltaAngle;
			// newAngle is too far left
			//if ( newAngle < TargetAngle )
			//{
			//	if ( TargetAngle <= CurrentAngle )
			//	{ newAngle	= TargetAngle; }
			//}
			//// newAngle is too far right
			//else if ( newAngle > TargetAngle )
			//{
			//	if ( TargetAngle >= CurrentAngle )
			//	{ newAngle	= TargetAngle; }
			//}

			float newAngle			= 0.0f;
			// CurrentAngle is too far left
			if ( CurrentAngle < TargetAngle )
			{
				if ( CurrentAngle + deltaAngle > TargetAngle )
				{ newAngle	= TargetAngle; }
				else
				{ newAngle	= CurrentAngle + deltaAngle; }
			}
			// CurrentAngle is too far right
			else if ( CurrentAngle > TargetAngle )
			{
				if ( CurrentAngle + deltaAngle < TargetAngle )
				{ newAngle	= TargetAngle; }
				else
				{ newAngle	= CurrentAngle + deltaAngle; }
			}

			switch ( Type )
			{
				case RotationType::Roll:
					CurrentRotation.Roll	= newAngle;
					break;
				case RotationType::Pitch:
					CurrentRotation.Pitch	= newAngle;
					break;
				case RotationType::Yaw:
					CurrentRotation.Yaw		= newAngle;
					break;
			}
		}
	}
	Component->SetRelativeRotation( CurrentRotation.Quaternion() );
	//Component->SetWorldRotation( CurrentRotation );
}

void UAITargetLogic::RotateComponent2( FRotator DiffInRotation, USceneComponent* Component, float DeltaTime, RotationType Type )
{
	FRotator CurrentRotation	= Component->RelativeRotation;
	float diffAngle				= 0.0f;
	float currentAngle			= 0.0f;
	float rotationMinimumToUse	= 0.0f;
	float rotationMaximumToUse	= 0.0f;
	float angluarSpeed			= 0.0f;

	switch ( Type )
	{
		case RotationType::Roll:
			diffAngle				= DiffInRotation.Roll;
			currentAngle			= CurrentRotation.Roll;
			rotationMinimumToUse	= RotationMinumum.Roll;
			rotationMaximumToUse	= RotationMaximum.Roll;
			angluarSpeed			= AngularVelocity.Roll;
			break;
		case RotationType::Pitch:
			diffAngle				= DiffInRotation.Pitch;
			currentAngle			= CurrentRotation.Pitch;
			rotationMinimumToUse	= RotationMinumum.Pitch;
			rotationMaximumToUse	= RotationMaximum.Pitch;
			angluarSpeed			= AngularVelocity.Pitch;
			break;
		case RotationType::Yaw:
			diffAngle				= DiffInRotation.Yaw;
			currentAngle			= CurrentRotation.Yaw;
			rotationMinimumToUse	= RotationMinumum.Yaw;
			rotationMaximumToUse	= RotationMaximum.Yaw;
			angluarSpeed			= AngularVelocity.Yaw;
			break;
	}

	// ensure that the weapon does not rotate by the big angle when sign changes for diffAngle
	if ( diffAngle > 180.0f )
	{ diffAngle		-= 360.0f; }
	else if ( diffAngle < -180.0f )
	{ diffAngle		+= 360.0f; }
	// ensure that the target angle is within rotation limits
	float targetAngle	= currentAngle + diffAngle;
	if ( targetAngle > rotationMaximumToUse )
	{ targetAngle		= rotationMaximumToUse; }
	if ( targetAngle < rotationMinimumToUse )
	{ targetAngle		= rotationMinimumToUse; }
	// correct the direction of rotation
	float deltaAngle	= DeltaTime * angluarSpeed;
	if ( diffAngle < 0.0f )
	{ deltaAngle		*= -1; }
	// calculate the new angle for this component
	float newAngle		= 0.0f;
	// CurrentAngle is too far left
	if ( currentAngle < targetAngle )
	{
		if ( currentAngle + deltaAngle > targetAngle )
		{ newAngle	= targetAngle; }
		else
		{ newAngle	= currentAngle + deltaAngle; }
	}
	// CurrentAngle is too far right
	else if ( currentAngle > targetAngle )
	{
		if ( currentAngle + deltaAngle < targetAngle )
		{ newAngle	= targetAngle; }
		else
		{ newAngle	= currentAngle + deltaAngle; }
	}

	switch ( Type )
	{
		case RotationType::Roll:
			CurrentRotation.Roll	= newAngle;
			break;
		case RotationType::Pitch:
			CurrentRotation.Pitch	= newAngle;
			break;
		case RotationType::Yaw:
			CurrentRotation.Yaw		= newAngle;
			break;
	}
	Component->SetRelativeRotation( CurrentRotation.Quaternion() );
}

void UAITargetLogic::SetStartingRotator( USceneComponent* Component )
{
	if ( nullptr != Component )
	{ StartingRotators.push_back( Component->GetComponentRotation() ); }
	else
	{ StartingRotators.push_back( FRotator::ZeroRotator ); }
}

void UAITargetLogic::OnTargetDeath( AActor* KilledActor, AController* KilledBy )
{
	Target = nullptr;
}

void UAITargetLogic::OnTargetDestroyed (AActor* DestroyedActor)
{
	Target = nullptr;
}
