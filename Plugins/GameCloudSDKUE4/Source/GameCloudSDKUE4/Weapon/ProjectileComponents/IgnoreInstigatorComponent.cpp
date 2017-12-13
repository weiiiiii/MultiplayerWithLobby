// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "IgnoreInstigatorComponent.h"
// include STL
// includes Engine files
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
// include plugin files
// includes third party files
// includes project files
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

// Sets default values for this component's properties
UIgnoreInstigatorComponent::UIgnoreInstigatorComponent()
{
}

void UIgnoreInstigatorComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner	= GetOwner();
	if ( nullptr != owner )
	{
		UShapeComponent* collider	= Cast<UShapeComponent>( owner->GetComponentByClass( UShapeComponent::StaticClass() ) );
		if ( collider->IsA( UBoxComponent::StaticClass() ) )
		{
			UBoxComponent* boxCollider	= Cast<UBoxComponent>( collider );
			UBoxComponent* newCollider	= NewObject<UBoxComponent>( this, "IgnoreCollider" );
			newCollider->RegisterComponent();
			newCollider->SetRelativeScale3D( boxCollider->RelativeScale3D );
			newCollider->SetBoxExtent( boxCollider->GetUnscaledBoxExtent() );
			newCollider->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
			newCollider->SetCollisionResponseToAllChannels( ECR_Overlap );
			newCollider->AttachToComponent( this, FAttachmentTransformRules( EAttachmentRule::KeepRelative, true ) );

			newCollider->OnComponentEndOverlap.AddDynamic( this, &UIgnoreInstigatorComponent::OnEndOverlap );
		}
		else if ( collider->IsA( USphereComponent::StaticClass() ) )
		{
			USphereComponent* sphereCollider	= Cast<USphereComponent>( collider );
			USphereComponent* newCollider		= NewObject<USphereComponent>( owner, "IgnoreCollider" );
			newCollider->RegisterComponent();
			newCollider->SetRelativeScale3D( sphereCollider->RelativeScale3D );
			newCollider->SetSphereRadius( sphereCollider->GetUnscaledSphereRadius() );
			newCollider->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
			newCollider->SetCollisionResponseToAllChannels( ECR_Overlap );
			newCollider->AttachToComponent( this, FAttachmentTransformRules( EAttachmentRule::KeepRelative, true ) );

			newCollider->OnComponentEndOverlap.AddDynamic( this, &UIgnoreInstigatorComponent::OnEndOverlap );
		}
	}
}

void UIgnoreInstigatorComponent::OnEndOverlap( class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	AActor* owner	= GetOwner();
	if ( nullptr != owner )
	{
		if ( OtherActor == owner )
		{
			//this->RemoveFromRoot();
			this->DestroyComponent( false );
			UCommonFunctions::PrintStringOnScreen( 5.0f, FColor::Red, "Component has left owned, removing!" );
		}
	}
}
