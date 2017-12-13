//// Fill out your copyright notice in the Description page of Project Settings.
//
//// fixed first include
//#include "ApplyDamageEvent.h"
//// include STL
//// includes Engine files
//#include "Components/ShapeComponent.h"
//#include "Kismet/GameplayStatics.h"
//#include "GameFramework/Pawn.h"
//#include "GameFramework/Controller.h"
//// include plugin files
//// includes third party files
//// includes project files
//
//UApplyDamageEvent::UApplyDamageEvent()
//{
//	PrimaryComponentTick.bCanEverTick = true;
//}
//
//void UApplyDamageEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	OnOverlapUpdate();
//}
//
//
//
//
//
//void UApplyDamageEvent::BeginPlay()
//{
//	Super::BeginPlay();
//	AActor* owner = GetOwner();
//	if ( owner )
//	{
//		UShapeComponent* shapeComponent = Cast<UShapeComponent>( owner->GetComponentByClass( UShapeComponent::StaticClass() ) );
//		if ( shapeComponent )
//		{
//			// Getting physical material surface in OnHit() without setting this as true will return nullptr
//			shapeComponent->bReturnMaterialOnMove = true;
//			if ( DamageOnHit )
//				shapeComponent->OnComponentHit.AddDynamic( this, &UApplyDamageEvent::OnHit );
//			if ( DamageOnBeginOverlap )
//				shapeComponent->OnComponentBeginOverlap.AddDynamic( this, &UApplyDamageEvent::OnBeginOverlap );
//			if ( DamageOnEndOverlap )
//				shapeComponent->OnComponentEndOverlap.AddDynamic( this, &UApplyDamageEvent::OnEndOverlap );
//		}
//	}
//}
//
//void UApplyDamageEvent::OnHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit )
//{
//	AActor* owner = GetOwner();
//	check( nullptr != owner );
//	APawn* instigator = owner->GetInstigator();
//	check( nullptr != instigator );
//	AController* controller = instigator->Controller;
//
//	ApplyAllDamage( Hit, controller, owner );
//}
//
//void UApplyDamageEvent::OnBeginOverlap( class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
//{
//	AActor* owner = GetOwner();
//	check( nullptr != owner );
//	APawn* instigator = owner->GetInstigator();
//	check( nullptr != instigator );
//	AController* controller = instigator->Controller;
//	ApplyAllDamage( SweepResult, controller, owner );
//}
//
//void UApplyDamageEvent::OnOverlapUpdate()
//{
//
//}
//
//void UApplyDamageEvent::OnEndOverlap( class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
//{
//
//}
//
//void UApplyDamageEvent::ApplyAllDamage( FHitResult Hit, AController* EventInstigatorController, AActor* DamageCauser )
//{
//	if ( ApplyDamage.IsEnabled )
//		UGameplayStatics::ApplyDamage( Hit.GetActor(), ApplyDamage.BaseDamage, EventInstigatorController,
//		DamageCauser, ApplyDamage.DamageType );
//
//	if ( ApplyPointDamage.IsEnabled )
//		UGameplayStatics::ApplyPointDamage( Hit.GetActor(), ApplyPointDamage.BaseDamage, Hit.ImpactNormal, Hit,
//		EventInstigatorController, DamageCauser, ApplyPointDamage.DamageType );
//
//	if ( ApplyRadialDamage.IsEnabled )
//	{
//// 		UGameplayStatics::ApplyRadialDamage( DamageCauser->GetWorld(), ApplyRadialDamage.BaseDamage, Hit.ImpactPoint,
//// 		ApplyRadialDamage.DamageRadius, nullptr, TArray<AActor*>(), DamageCauser, EventInstigatorController,
//// 		ApplyRadialDamage.DoFullDamage );
////		UCommonFunctions::PrintFStringOnScreen( 10.f, FColor::Orange, GetOwner()->GetActorLocation().ToCompactString() );
//		bool result = UGameplayStatics::ApplyRadialDamage( Cast<UObject>( DamageCauser->GetWorld() ), ApplyRadialDamage.BaseDamage, GetOwner()->GetActorLocation(),
//			ApplyRadialDamage.DamageRadius, ApplyRadialDamage.DamageType, TArray<AActor*>(), DamageCauser, EventInstigatorController,
//			ApplyRadialDamage.DoFullDamage );
//
//// 		UCommonFunctions::PrintFStringOnScreen( 5.0f, FColor::Red, FString::SanitizeFloat( ApplyRadialDamage.BaseDamage ) );
//// 		if ( result )
//// 			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "UApplyDamageEvent : Successfully applied damage" );
//// 		else
//// 			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "UApplyDamageEvent : Damage was not applied" );
//
//		//DrawDebugSphere( DamageCauser->GetWorld(), GetOwner()->GetActorLocation(), ApplyRadialDamage.DamageRadius, 20, FColor::Blue, true, 10.0f );
//	}
//
//	if ( ApplyRadialDamageWithFalloff.IsEnabled )
//		UGameplayStatics::ApplyRadialDamageWithFalloff( Cast<UObject>( DamageCauser->GetWorld() ), ApplyRadialDamageWithFalloff.BaseDamage,
//		ApplyRadialDamageWithFalloff.MinimumDamage, Hit.ImpactPoint, ApplyRadialDamageWithFalloff.DamageInnerRadius,
//		ApplyRadialDamageWithFalloff.DamageOuterRadius, ApplyRadialDamageWithFalloff.DamageFalloff, ApplyRadialDamageWithFalloff.DamageType, TArray<AActor*>(),
//		DamageCauser, EventInstigatorController );
//
//	//DebugDamage.PrintOnEvent( GetOwner(), Hit );
//}
//
