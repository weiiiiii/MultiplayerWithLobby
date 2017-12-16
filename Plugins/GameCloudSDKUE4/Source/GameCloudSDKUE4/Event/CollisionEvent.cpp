// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "CollisionEvent.h"
// include STL
// includes Engine files
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/FX/FFXDecalStruct.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"
#include "GameCloudSDKUE4/Weapon/ProjectileComponents/ParticleTrail.h"
//#include "GameCloudSDKUE4/Weapon/ProjectileComponents/IgnoreInstigatorComponent.h"

UCollisionEvent::UCollisionEvent()
	//: IgnoreInstigatorOnStart( false )
	: SoundFadeOutDuration( 0.0f )

	, DamageOnHit( false )
	, DamageOnBeginOverlap( false )
	, DamageOnOverlapUpdate( false )
	, DamageOnEndOverlap( false )
	, ApplyDamage()
	, ApplyPointDamage()
	, ApplyRadialDamage()
	, ApplyRadialDamageWithFalloff()

	, EnableOnHit( false )
	, SpawnOnHit()
	, DecalOnHit( nullptr )
	, DestroyActorOnHit( false )

	, EnableOnBeginOverlap( false )
	, SpawnOnBeginOverlap()
	, SpawnDecalOnBeginOverlap( false )
	, DestroyActorOnBeginOverlap( nullptr )
	, DebugOnBeginOverlap()

	, EnableOnOverlapUpdate( false )
	, SpawnOnOverlapUpdate()
	, SpawnDecalOnOverlapUpdate( false )
	, DestroyActorOnOverlapUpdate( nullptr )
	, DebugOnOverlapUpdate()

	, EnableOnEndOverlap( false )
	, DestroyActorOnEndOverlap( nullptr )
	, DebugOnEndOverlap()

	, shapeComponent( nullptr )
	, audioComponent( nullptr )
	, ShouldDestroy( false )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCollisionEvent::OnHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit )
{
	DebugOnHit.PrintOnHitEvent( "On Component Hit", Hit );

	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	if ( DamageOnHit )
	{
		APawn* instigator		= nullptr;
		AController* controller	= nullptr;

		instigator = owner->GetInstigator();
		if ( nullptr != instigator )
		{ controller = instigator->Controller; }
		ApplyAllDamage( Hit, controller, owner );
	}

	for ( auto spawn : SpawnOnHit )
		spawn.SpawnObjects( owner );

	spawnDecal( DecalOnHit, Hit );

	if ( DestroyActorOnHit )
		ShouldDestroy	= true;
}

void UCollisionEvent::OnBeginOverlap( class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	DebugOnBeginOverlap.PrintOnHitEvent( "On Begin Overlap", SweepResult );

	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	if ( DamageOnBeginOverlap )
	{
		APawn* instigator		= nullptr;
		AController* controller	= nullptr;

		instigator = owner->GetInstigator();
		if ( nullptr != instigator )
		{ controller = instigator->Controller; }
		ApplyAllDamage( SweepResult, controller, owner );
	}

	for ( auto spawn : SpawnOnBeginOverlap )
		spawn.SpawnObjects( owner );

	//UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "OnBeginOverlap : Spawning Decal" );
	spawnDecal( DecalOnOverlap, SweepResult );

	if ( DestroyActorOnBeginOverlap )
		ShouldDestroy	= true;
}

void UCollisionEvent::OnOverlapUpdate()
{
	if ( !EnableOnOverlapUpdate )
		return;

	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	if ( DamageOnOverlapUpdate )
	{
		// some how apply damage if required
	}

	const TArray<FOverlapInfo>& infos = shapeComponent->GetOverlapInfos();
	for ( auto info : infos )
	{
		for ( auto spawn : SpawnOnOverlapUpdate )
			spawn.SpawnObjects( owner );

		spawnDecal( DecalOnOverlap, info.OverlapInfo );

		DebugOnOverlapUpdate.PrintOnHitEvent( "On Overlap Update", info.OverlapInfo );
	}

	if ( DestroyActorOnOverlapUpdate )
		ShouldDestroy	= true;
}

void UCollisionEvent::OnEndOverlap( class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	if ( DamageOnEndOverlap )
	{
		// some how apply damage if required
	}

	if ( DestroyActorOnEndOverlap )
		ShouldDestroy	= true;
	//DebugOnEndOverlap.PrintOnEvent( "On End Overlap", FHitResult( 1 ) );
}

void UCollisionEvent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	OnOverlapUpdate();

	if ( ShouldDestroy )
	{ destroyActor(); }
}





void UCollisionEvent::BeginPlay()
{
	Super::BeginPlay();
	AActor* owner = GetOwner();
	if ( owner )
	{
		// shapeComponent will be the base component
		shapeComponent = Cast<UShapeComponent>( owner->GetComponentByClass( UShapeComponent::StaticClass() ) );
		if ( shapeComponent )
		{
			// Getting physical material surface in OnHit() without setting this as true will return nullptr
			shapeComponent->bReturnMaterialOnMove = true;
			if ( EnableOnHit )
				shapeComponent->OnComponentHit.AddDynamic( this, &UCollisionEvent::OnHit );
			if ( EnableOnBeginOverlap )
				shapeComponent->OnComponentBeginOverlap.AddDynamic( this, &UCollisionEvent::OnBeginOverlap );
			if ( EnableOnEndOverlap )
				shapeComponent->OnComponentEndOverlap.AddDynamic( this, &UCollisionEvent::OnEndOverlap );

			// find audio component from shape component's children
			TArray<USceneComponent*> children;
			shapeComponent->GetChildrenComponents( false, children );
			for ( USceneComponent* child : children )
			{
				audioComponent = Cast<UAudioComponent>( child );
				if ( audioComponent )
					break;
			}

			// TODO : Try spawning this after fixing error in IgnoreInstigatorComponent
			//if ( IgnoreInstigatorOnStart )
			//{
			//	USceneComponent* rootComponent	= owner->GetRootComponent();
			//	UIgnoreInstigatorComponent* component	= NewObject<UIgnoreInstigatorComponent>( owner, "IgnoreInstigatorComponent" );
			//	component->AttachToComponent( rootComponent, FAttachmentTransformRules( EAttachmentRule::KeepRelative, true ) );
			//	component->RegisterComponent();
			//}
		}
	}
}

void UCollisionEvent::ApplyAllDamage( FHitResult Hit, AController* EventInstigatorController, AActor* DamageCauser )
{
	if ( ApplyDamage.IsEnabled )
		UGameplayStatics::ApplyDamage( Hit.GetActor(), ApplyDamage.BaseDamage, EventInstigatorController,
		DamageCauser, ApplyDamage.DamageType );

	if ( ApplyPointDamage.IsEnabled )
		UGameplayStatics::ApplyPointDamage( Hit.GetActor(), ApplyPointDamage.BaseDamage, Hit.ImpactNormal, Hit,
		EventInstigatorController, DamageCauser, ApplyPointDamage.DamageType );

	if ( ApplyRadialDamage.IsEnabled )
	{
		const ECollisionChannel & Channel = UCollisionProfile::Get()->ConvertToCollisionChannel( false, ApplyRadialDamage.DamagePreventionChannel );
		bool result = UGameplayStatics::ApplyRadialDamage( Cast<UObject>( DamageCauser->GetWorld() ), ApplyRadialDamage.BaseDamage, GetOwner()->GetActorLocation(),
			ApplyRadialDamage.DamageRadius, ApplyRadialDamage.DamageType, TArray<AActor*>(), DamageCauser, EventInstigatorController,
			ApplyRadialDamage.DoFullDamage, Channel );

		// 		UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, FString::SanitizeFloat( ApplyRadialDamage.BaseDamage ) );
		// 		if ( result )
		// 			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "UApplyDamageEvent : Successfully applied damage" );
		// 		else
		// 			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "UApplyDamageEvent : Damage was not applied" );

		//DrawDebugSphere( DamageCauser->GetWorld(), GetOwner()->GetActorLocation(), ApplyRadialDamage.DamageRadius, 20, FColor::Blue, true, 10.0f );
	}

	if ( ApplyRadialDamageWithFalloff.IsEnabled )
	{
		const ECollisionChannel & Channel = UCollisionProfile::Get()->ConvertToCollisionChannel( false, ApplyRadialDamageWithFalloff.DamagePreventionChannel );
		UGameplayStatics::ApplyRadialDamageWithFalloff( Cast<UObject>( DamageCauser->GetWorld() ), ApplyRadialDamageWithFalloff.BaseDamage,
			ApplyRadialDamageWithFalloff.MinimumDamage, Hit.ImpactPoint, ApplyRadialDamageWithFalloff.DamageInnerRadius,
			ApplyRadialDamageWithFalloff.DamageOuterRadius, ApplyRadialDamageWithFalloff.DamageFalloff, ApplyRadialDamageWithFalloff.DamageType, TArray<AActor*>(),
			DamageCauser, EventInstigatorController, Channel );
	}
}

void UCollisionEvent::spawnDecal( UDataTable* decalTable, FHitResult hitResult )
{
	if ( nullptr == decalTable )
		return;
	
	TWeakObjectPtr<UPhysicalMaterial> physMat = hitResult.PhysMaterial;
	if ( nullptr == physMat )
	{
		//UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Spawning Decal : Physics material is nullptr" );
		return;
	}

	const FString & surfaceTypeName = UCommonFunctions::GetEnumValueToString( "EPhysicalSurface", physMat->SurfaceType );
	FFXDecalStruct* decalStruct = decalTable->FindRow<FFXDecalStruct>( FName( *surfaceTypeName ), "" );
	if ( nullptr == decalStruct )
	{
		//UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Spawning Decal : Decal data table row does not match FXDecalStruct!" );
		return;
	}

	AActor* owner = GetOwner();
	if ( nullptr == owner )
	{
		//UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Spawning Decal : Owner does not exist" );
		return;
	}

	FVector scale = FVector( 1.0f );
	if ( shapeComponent )
		scale = shapeComponent->GetComponentScale();
	decalStruct->SpawnImpactParticleFX( owner->GetWorld(), hitResult, scale );
	decalStruct->SpawnImpactDecals( owner->GetWorld(), hitResult, scale );
	decalStruct->PlayImpactSound( owner->GetWorld(), hitResult );
	decalStruct->SpawnDebrisParticleFX( owner->GetWorld(), hitResult, scale );
}

void UCollisionEvent::destroyActor()
{
	AActor* owner = GetOwner();
	if ( nullptr == owner )
		return;

	TArray<UActorComponent*> components;
	components = owner->GetComponentsByClass( UParticleTrail::StaticClass() );

	// fade sound
	if ( audioComponent )
		audioComponent->FadeOut( SoundFadeOutDuration, 0 );

	// owner does not have any particle trails to deactivate, proceed to destroy
	if ( 0 == components.Num() )
	{
		owner->Destroy();
		return;
	}

	bool areTrailsDeactivated = false;
	for ( UActorComponent* component : components )
	{
		UParticleTrail* trail = Cast<UParticleTrail>( component );
		if ( nullptr == component )
			continue;

		if ( trail->DeactivateTrails() )
			areTrailsDeactivated = true;
	}

	if ( !areTrailsDeactivated )
	{
		owner->Destroy();
		return;
	}

	// hide 1st static mesh
	// hiding owner will hide particle trail irregardless of whether the trail has been detached or not
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>( owner->GetComponentByClass( UStaticMeshComponent::StaticClass() ) );
	if ( mesh )
		mesh->SetHiddenInGame( true );
	// disable collision
	UShapeComponent* collision = Cast<UShapeComponent>( owner->GetComponentByClass( UShapeComponent::StaticClass() ) );
	if ( collision )
	{
		collision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
		collision->SetHiddenInGame( true );
	}
}
