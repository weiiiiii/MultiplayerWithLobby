//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//// include STL
//// includes Engine files
//#include "Components/ActorComponent.h"
//#include "Kismet/KismetSystemLibrary.h"
//// include plugin files
//// includes third party files
//// includes project files
//#include "GameCloudSDKUE4/Common/CommonFunctions.h"
//// fixed final include
//#include "ApplyDamageEvent.generated.h"
//
//class UDamageType;
//
//USTRUCT( BlueprintType )
//struct FApplyDamageStruct
//{
//	GENERATED_BODY()
//
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		bool IsEnabled;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float BaseDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		TSubclassOf<UDamageType> DamageType;
//
//	FApplyDamageStruct()
//		: IsEnabled( false )
//		, BaseDamage( 0.0f )
//		, DamageType( nullptr )
//	{
//
//	}
//};
//
//USTRUCT( BlueprintType )
//struct FApplyRadialDamageStruct
//{
//	GENERATED_BODY()
//
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		bool IsEnabled;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float BaseDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float DamageRadius;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		bool DoFullDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		TSubclassOf<UDamageType> DamageType;
//
//	FApplyRadialDamageStruct()
//		: IsEnabled( false )
//		, BaseDamage( 0.0f )
//		, DamageRadius( 0.0f )
//		, DoFullDamage( false )
//		, DamageType( nullptr )
//	{
//
//	}
//};
//
//USTRUCT( BlueprintType )
//struct FApplyRadialDamageWithFalloffStruct
//{
//	GENERATED_BODY()
//
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		bool IsEnabled;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float BaseDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float MinimumDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float DamageInnerRadius;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float DamageOuterRadius;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float DamageFalloff;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		TSubclassOf<UDamageType> DamageType;
//
//	FApplyRadialDamageWithFalloffStruct()
//		: IsEnabled( false )
//		, BaseDamage( 0.0f )
//		, MinimumDamage( 0.0f )
//		, DamageInnerRadius( 0.0f )
//		, DamageOuterRadius( 0.0f )
//		, DamageFalloff( 0.0f )
//		, DamageType( nullptr )
//	{
//
//	}
//};
//
//USTRUCT( BlueprintType )
//struct FDebugDamageStruct
//{
//	GENERATED_BODY()
//
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		bool PrintOnScreen;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		FColor ScreenColour;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite )
//		float Duration;
//
//	FDebugDamageStruct()
//		: PrintOnScreen( false )
//		, ScreenColour( FColor::Red )
//		, Duration( 2.0f )
//	{
//
//	}
//
//	void PrintOnEvent( AActor* actor, FHitResult Hit )
//	{
//		FString message = "";
//		if ( nullptr != actor )
//		{
//			FString actorName = UKismetSystemLibrary::GetDisplayName( Cast<UObject>( actor ) );
//			message += actorName;	
//		}
//		else
//			message += "Nameless";
//
//		message += " dealt damage to ";
//
//		if ( Hit.GetActor() )
//		{
//			FString actorName = UKismetSystemLibrary::GetDisplayName( Cast<UObject>( Hit.GetActor() ) );
//			message += actorName;
//		}
//
//		if ( PrintOnScreen )
//			UCommonFunctions::PrintFStringOnScreen( Duration, ScreenColour, message );
//		// 		if ( PrintInLog )
//		// 			UE_LOG( LogTemp, Warning, TEXT( "%s" ),  message );
//	}
//};
//
//
//UCLASS( ClassGroup = ( GameCloudSDKUE4 ), meta = ( BlueprintSpawnableComponent ) )
//class GAMECLOUDSDKUE4_API UApplyDamageEvent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "When enabled, component will apply damage on event, OnComponentHit, of ShapeComponent owned by owner" ) )
//		bool DamageOnHit;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "When enabled, component will apply damage on event, OnComponentOverlapBegin, of ShapeComponent owned by owner" ) )
//		bool DamageOnBeginOverlap;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "TBC" ) )
//		bool DamageOnOverlapUpdate;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "TBC" ) )
//		bool DamageOnEndOverlap;
//// 	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event" ) )
//// 		FDebugDamageStruct DebugDamage;
//
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "Values to use when calling Apply Damage" ) )
//		FApplyDamageStruct ApplyDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "Values to use when calling Apply Point Damage" ) )
//		FApplyDamageStruct ApplyPointDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "Values to use when calling Apply Radial Damage" ) )
//		FApplyRadialDamageStruct ApplyRadialDamage;
//	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( Category = "GameCloudSDKUE4|Event", Tooltip = "Values to use when calling Apply Radial Damage With Falloff" ) )
//		FApplyRadialDamageWithFalloffStruct ApplyRadialDamageWithFalloff;
//
//protected:
//
//
//public:
//	UApplyDamageEvent();
//	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
//
//protected:
//	virtual void BeginPlay() override;
//	UFUNCTION()
//		void OnHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit );
//	UFUNCTION()
//		void OnBeginOverlap( class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
//	UFUNCTION()
//		void OnOverlapUpdate();
//	UFUNCTION()
//		void OnEndOverlap( class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
//
//	UFUNCTION()
//		void ApplyAllDamage( FHitResult Hit, AController* EventInstigatorController, AActor* DamageCauser );
//};
