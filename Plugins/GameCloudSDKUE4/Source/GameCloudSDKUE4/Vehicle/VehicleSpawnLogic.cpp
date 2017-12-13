// Fill out your copyright notice in the Description page of Project Settings.

// fixed first include
#include "VehicleSpawnLogic.h"
// include STL
// includes Engine files
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
// include plugin files
// includes third party files
// includes project files
#include "VehiclePart.h"
#include "GameCloudSDKUE4/Common/CommonFunctions.h"

UVehicleSpawnLogic::UVehicleSpawnLogic()
	: VehicleData( nullptr )

	, isFromMenu( false )
{
}

TMap<VehiclePartType, int> UVehicleSpawnLogic::GetSpawnedIndices()
{
	TMap<VehiclePartType, int> toReturn;
	for ( std::unordered_map<VehiclePartType, int>::iterator indexIter = spawnedIndices.begin(); indexIter != spawnedIndices.end(); ++indexIter )
		toReturn.Add( indexIter->first, indexIter->second );
	return toReturn;
}

TArray<AActor*> UVehicleSpawnLogic::GetPartsByType( VehiclePartType type )
{
	TArray<AActor*> toReturn;
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator partsIter = spawnedParts.find( type );
	if ( spawnedParts.end() == partsIter )
		return toReturn;

	std::vector<AActor*> partsVector = partsIter->second;
	for ( AActor* vehiclePart : partsVector )
		toReturn.Add( vehiclePart );
	return toReturn;
}

void UVehicleSpawnLogic::PreviewPart( VehiclePartType type, bool isPrev )
{
	// get possible indices
	std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>>::iterator classesIter = spawnableClasses.find( type );
	std::vector<TSubclassOf<AActor>> classes = classesIter->second;
	// nothing to preview with
	if ( classes.size() <= 1 )
		return;

	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator leftIter = standbyPartsLeft.find( type );
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator spawnedIter = spawnedParts.find( type );
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator rightIter = standbyPartsRight.find( type );
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator viewIter = viewingParts.find( type );

	std::unordered_map<VehiclePartType, int>::iterator leftIndexIter = standbyIndicesLeft.find( type );
	std::unordered_map<VehiclePartType, int>::iterator spawnedIndexIter = spawnedIndices.find( type );
	std::unordered_map<VehiclePartType, int>::iterator rightIndexIter = standbyIndicesRight.find( type );
	std::unordered_map<VehiclePartType, int>::iterator viewIndexIter = viewingIndices.find( type );

	// no viewing part(s) exist, set current spawned parts as viewing parts
	if ( viewingParts.end() == viewIter )
	{
		hideActors( spawnedIter->second, true );
		viewingIndices.insert( { type, spawnedIndexIter->second } );
		spawnPart( type, PartStatus::VIEWING, true );

		viewIter = viewingParts.find( type );
		viewIndexIter = viewingIndices.find( type );
	}

	// decrease index, shift actors from left to right
	if ( isPrev )
	{		
		hideActors( viewIter->second, true );
		hideActors( leftIter->second, false );

		// shift actors right
		replacePart( type, PartStatus::STANDBY_RIGHT, viewIter->second, true );
		replacePart( type, PartStatus::VIEWING, leftIter->second, false );

		// shift indices
		replaceIndex( type, PartStatus::STANDBY_RIGHT, viewIndexIter->second );
		replaceIndex( type, PartStatus::VIEWING, leftIndexIter->second );

		// update left index
		int leftIndex = leftIndexIter->second - 1;
		warpValueInRange( leftIndex, 0, classes.size() - 1 );
		replaceIndex( type, PartStatus::STANDBY_LEFT, leftIndex );
		// spawn standby left
		spawnPart( type, PartStatus::STANDBY_LEFT, false );
	}
	// increase index, shift actors from right to left
	else
	{
		// hide & show
		hideActors( viewIter->second, true );
		hideActors( rightIter->second, false );

		// shift actors left
		replacePart( type, PartStatus::STANDBY_LEFT, viewIter->second, true );
		replacePart( type, PartStatus::VIEWING, rightIter->second, false );

		// shift indices
		replaceIndex( type, PartStatus::STANDBY_LEFT, viewIndexIter->second );
		replaceIndex( type, PartStatus::VIEWING, rightIndexIter->second );
		leftIndexIter->second = viewIndexIter->second;
		viewIndexIter->second = rightIndexIter->second;

		// update right index
		int rightIndex = rightIndexIter->second + 1;
		warpValueInRange( rightIndex, 0, classes.size() - 1 );
		replaceIndex( type, PartStatus::STANDBY_RIGHT, rightIndex );
		// spawn standby right
		spawnPart( type, PartStatus::STANDBY_RIGHT, false );
	}

	// preview part has looped back to current seleceted part
	viewIndexIter = viewingIndices.find( type );
	if ( viewIndexIter->second == spawnedIndexIter->second )
	{
		removePart( type, PartStatus::VIEWING, true );
		viewingIndices.erase( type );
		hideActors( spawnedIter->second, false );
	}
}

void UVehicleSpawnLogic::ResetToDefault( VehiclePartType type )
{
	std::unordered_map<VehiclePartType, int>::iterator defaultIndexIter = defaultSpawnableIndices.find( type );
	// there MUST always be a default!
	checkf( defaultSpawnableIndices.end() != defaultIndexIter, *( "Default not found for " + UCommonFunctions::GetEnumValueToString( "VehiclePartType", type ) ) );

	int defaultIndex = defaultIndexIter->second;
	std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>>::iterator classesIter = spawnableClasses.find( type );
	std::vector<TSubclassOf<AActor>> classes = classesIter->second;

	// clear all parts
	removePart( type, PartStatus::SPAWNED, true );
	removePart( type, PartStatus::STANDBY_LEFT, true );
	removePart( type, PartStatus::STANDBY_RIGHT, true );
	removePart( type, PartStatus::VIEWING, true );

	// replace spawned
	int spawnedIndex = defaultIndex;
	replaceIndex( type, PartStatus::SPAWNED, spawnedIndex );
	// replace standby left
	int standbyIndexLeft = spawnedIndex - 1;
	warpValueInRange( standbyIndexLeft, 0, classes.size() - 1 );
	if ( standbyIndexLeft != spawnedIndex )
		replaceIndex( type, PartStatus::STANDBY_LEFT, standbyIndexLeft );
	// replace standby right
	int standbyIndexRight = spawnedIndex + 1;
	warpValueInRange( standbyIndexRight, 0, classes.size() - 1 );
	if ( standbyIndexRight != spawnedIndex )
		replaceIndex( type, PartStatus::STANDBY_RIGHT, standbyIndexRight );

	spawnPart( type, PartStatus::SPAWNED, true );
	spawnPart( type, PartStatus::STANDBY_LEFT, true );
	spawnPart( type, PartStatus::STANDBY_RIGHT, true );
}

void UVehicleSpawnLogic::SelectPreviewPart( VehiclePartType type )
{
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator viewingPartsIter = viewingParts.find( type );
	// viewing parts for this type does not exist, nothing to select, end function
	if ( viewingParts.end() == viewingPartsIter )
		return;
	//////////////////////////////////////////////////////////////////////////
	// condition above, assert even if this might be used by developers who do not know c++?
	//////////////////////////////////////////////////////////////////////////

	std::unordered_map<VehiclePartType, int>::iterator leftIndexIter = standbyIndicesLeft.find( type );
	std::unordered_map<VehiclePartType, int>::iterator spawnedIndexIter = spawnedIndices.find( type );
	std::unordered_map<VehiclePartType, int>::iterator rightIndexIter = standbyIndicesRight.find( type );

	bool destroySpawned = ( leftIndexIter->second != spawnedIndexIter->second && rightIndexIter->second != spawnedIndexIter->second );
	
	// remove spawned parts
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator spawnedPartsIter = spawnedParts.find( type );
	if ( spawnedParts.end() != spawnedPartsIter )
	{
		if ( destroySpawned )
		{
			std::vector<AActor*> spawnedPartsArray = spawnedPartsIter->second;
			for ( int i = 0; i < spawnedPartsArray.size(); i++ )
				spawnedPartsArray[i]->Destroy();
		}
		spawnedParts.erase( type );
	}	

	// add array of viewing parts to spawned parts
	spawnedParts.insert( { type, viewingPartsIter->second } );
	// remove viewing parts
	viewingParts.erase( type );
	// replace spawned indices
	replaceIndex( type, PartStatus::SPAWNED, viewingIndices.find( type )->second );
	// remove viewing indices
	viewingIndices.erase( type );
}

void UVehicleSpawnLogic::SetupPlaceholders( USceneComponent* placeholdersParent )
{	
	TArray<USceneComponent*> children;
	placeholdersParent->GetChildrenComponents( false, children );
	for ( USceneComponent* child : children )
	{
		UVehiclePart* vehiclePart = Cast<UVehiclePart>( child );
		// show warning message that there exist a component that is not VehiclePart?
		if ( nullptr == vehiclePart )
			continue;

 		VehiclePartType type = vehiclePart->Type;
		std::unordered_map<VehiclePartType, std::vector<UVehiclePart*>>::iterator iter = placeholders.find( type );
		if ( placeholders.end() == iter )
		{
			placeholders.insert( { type, std::vector<UVehiclePart*>() } );
			iter = placeholders.find( vehiclePart->Type );
		}
		iter->second.push_back( vehiclePart );
	}
}

//void UVehicleSpawnLogic::SpawnParts( TMap<VehiclePartType, int> &PartIndices, bool DisableCollision )
void UVehicleSpawnLogic::SpawnParts( TArray<int> PartIndices, bool DisableCollision )
{
	isFromMenu = DisableCollision;

	if ( (int)VehiclePartType::INVALID_PART == PartIndices.Num() )
	{
		for ( int i = 0; i < (int)VehiclePartType::INVALID_PART; i++ )
		{
			VehiclePartType type = VehiclePartType( i );
			std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>>::iterator classesIter = spawnableClasses.find( type );
			std::vector<TSubclassOf<AActor>> classes = classesIter->second;

			//int index = *( PartIndices.Find( type ) );
			int index = PartIndices[i];
			warpValueInRange( index, 0, classes.size() - 1 );
			replaceIndex( type, PartStatus::SPAWNED, index );
		}
	}
	else
		spawnedIndices = defaultSpawnableIndices;

	// spawn parts
	for ( int i = 0; i < (int)VehiclePartType::INVALID_PART; i++ )
	{
		VehiclePartType type = VehiclePartType( i );
		spawnPart( type, PartStatus::SPAWNED, true );
	}

	if ( isFromMenu )
	{
		// prepare standbyIndicesLeft and standbyIndicesRight
		for ( int i = 0; i < (int)VehiclePartType::INVALID_PART; i++ )
		{
			VehiclePartType type = VehiclePartType( i );
			std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>>::iterator classesIter = spawnableClasses.find( type );
			std::vector<TSubclassOf<AActor>> classes = classesIter->second;
			int index = spawnedIndices.find( type )->second;

			int leftIndex = index - 1;
			warpValueInRange( leftIndex, 0, classes.size() - 1 );
			if ( leftIndex != index )
				replaceIndex( type, PartStatus::STANDBY_LEFT, leftIndex );

			int rightIndex = index + 1;
			warpValueInRange( rightIndex, 0, classes.size() - 1 );
			if ( rightIndex != index )
				replaceIndex( type, PartStatus::STANDBY_RIGHT, rightIndex );
		}

		// spawn standby parts
		for ( int i = 0; i < (int)VehiclePartType::INVALID_PART; i++ )
		{
			VehiclePartType type = VehiclePartType( i );
			spawnPart( type, PartStatus::STANDBY_LEFT, true );
			spawnPart( type, PartStatus::STANDBY_RIGHT, true );
		}
	}
}

void UVehicleSpawnLogic::SwapPart( VehiclePartType type, bool isPrev )
{
	// get possible indices
	std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>>::iterator classesIter = spawnableClasses.find( type );
	std::vector<TSubclassOf<AActor>> classes = classesIter->second;
	// nothing to swap with
	if ( classes.size() <= 1 )
		return;

	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator leftIter = standbyPartsLeft.find( type );
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator spawnedIter = spawnedParts.find( type );
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator rightIter = standbyPartsRight.find( type );

	std::unordered_map<VehiclePartType, int>::iterator leftIndexIter = standbyIndicesLeft.find( type );
	std::unordered_map<VehiclePartType, int>::iterator spawnedIndexIter = spawnedIndices.find( type );
	std::unordered_map<VehiclePartType, int>::iterator rightIndexIter = standbyIndicesRight.find( type );

	// decrease index, shift actors from left to right
	if ( isPrev )
	{
		// hide & show
		hideActors( spawnedIter->second, true );
		hideActors( leftIter->second, false );

		// shift actors right
		replacePart( type, PartStatus::STANDBY_RIGHT, spawnedIter->second, true );
		replacePart( type, PartStatus::SPAWNED, leftIter->second, false );

		// shift indices
		replaceIndex( type, PartStatus::STANDBY_RIGHT, spawnedIndexIter->second );
		replaceIndex( type, PartStatus::SPAWNED, leftIndexIter->second );

		// update left index
		int leftIndex = leftIndexIter->second - 1;
		warpValueInRange( leftIndex, 0, classes.size() - 1 );
		replaceIndex( type, PartStatus::STANDBY_LEFT, leftIndex );
		// spawn standby left
		spawnPart( type, PartStatus::STANDBY_LEFT, false );
	}
	// increase index, shift actors from right to left
	else
	{
		// hide & show
		hideActors( spawnedIter->second, true );
		hideActors( rightIter->second, false );

		// shift actors left
		replacePart( type, PartStatus::STANDBY_LEFT, spawnedIter->second, true );
		replacePart( type, PartStatus::SPAWNED, rightIter->second, false );

		// shift indices
		replaceIndex( type, PartStatus::STANDBY_LEFT, spawnedIndexIter->second );
		replaceIndex( type, PartStatus::SPAWNED, rightIndexIter->second );

		// update right index
		int rightIndex = rightIndexIter->second + 1;
		warpValueInRange( rightIndex, 0, classes.size() - 1 );
		replaceIndex( type, PartStatus::STANDBY_RIGHT, rightIndex );
		// spawn standby right
		spawnPart( type, PartStatus::STANDBY_RIGHT, false );
	}
}

void UVehicleSpawnLogic::BeginPlay()
{
	Super::BeginPlay();
	loadData();
}
 
void UVehicleSpawnLogic::loadData()
{
	checkf( nullptr != VehicleData, *FString( "Vehicle data table is missing!" ) );
	TArray<FName> rowNames = VehicleData->GetRowNames();
 
	for ( int i = 0; i < (int)VehiclePartType::INVALID_PART; i++ )
		spawnableClasses.insert( { VehiclePartType( i ), std::vector<TSubclassOf<AActor>>() } );

	// set default
	{
		FVehiclePartsStruct* partsStruc = VehicleData->FindRow<FVehiclePartsStruct>( "Default", "VehicleSpawnLogic" );
		checkf( nullptr != partsStruc, *FString( "Vehicle data table must have row \"Default!\"" ) );
		
		for ( int i = 0; i < (int)VehiclePartType::INVALID_PART; i++ )
		{
			VehiclePartType type = VehiclePartType( i );
			TSubclassOf<AActor> classPointer = partsStruc->GetClassByType( type );
			spawnableClasses.find( type )->second.push_back( classPointer );
			defaultSpawnableIndices.insert( { type, 0 } );
		}
		rowNames.Remove( "Default" );
	}

	for ( int i = 0; i < rowNames.Num(); i++ )
	{
		FName name = rowNames[i];
		FVehiclePartsStruct* partsStruc = VehicleData->FindRow<FVehiclePartsStruct>( name, "VehicleSpawnLogic" );
		checkf( nullptr != partsStruc, *FString( "Vehicle data table is not created with Vehicle Parts Struct!" ) );
		
		for ( int j = 0; j < (int)VehiclePartType::INVALID_PART; j++ )
		{
			VehiclePartType type = VehiclePartType( j );
			TSubclassOf<AActor> classPointer = partsStruc->GetClassByType( type );
			if ( nullptr != classPointer )
				spawnableClasses.find( type )->second.push_back( classPointer );
		}
	}
}

void UVehicleSpawnLogic::hideActors( std::vector<AActor*> actors, bool isHiding )
{
	for ( AActor* actor : actors )
		actor->SetActorHiddenInGame( isHiding );
}

void UVehicleSpawnLogic::hideSpawnedParts( VehiclePartType type, bool isHiding )
{
	std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator spawnedPartsIter = spawnedParts.find( type );
	if ( spawnedParts.end() != spawnedPartsIter )
	{
		std::vector<AActor*> spawnedActors = spawnedPartsIter->second;
		for ( AActor* actor : spawnedActors )
			actor->SetActorHiddenInGame( isHiding );
	}
}

void UVehicleSpawnLogic::removePart( VehiclePartType type, PartStatus status, bool destroyExisting )
{
	std::unordered_map<VehiclePartType, std::vector<AActor*>> *container;
	switch ( status )
	{
		case UVehicleSpawnLogic::STANDBY_LEFT:
			container = &standbyPartsLeft;
			break;
		case UVehicleSpawnLogic::SPAWNED:
			container = &spawnedParts;
			break;
		case UVehicleSpawnLogic::STANDBY_RIGHT:
			container = &standbyPartsRight;
			break;
		case UVehicleSpawnLogic::VIEWING:
			container = &viewingParts;
			break;
		default:
			return;
	}

	if ( destroyExisting )
	{
		std::unordered_map<VehiclePartType, std::vector<AActor*>>::iterator actorsIter = container->find( type );
		if ( container->end() != actorsIter )
		{
			std::vector<AActor*> actors = actorsIter->second;
			for ( int i = 0; i < actors.size(); i++ )
				actors[i]->Destroy();
		}
	}

	container->erase( type );
}

void UVehicleSpawnLogic::replaceIndex( VehiclePartType type, PartStatus status, int newIndex )
{
	std::unordered_map<VehiclePartType, int> *container;
	switch ( status )
	{
		case UVehicleSpawnLogic::STANDBY_LEFT:
			container = &standbyIndicesLeft;
			break;
		case UVehicleSpawnLogic::SPAWNED:
			container = &spawnedIndices;
			break;
		case UVehicleSpawnLogic::STANDBY_RIGHT:
			container = &standbyIndicesRight;
			break;
		case UVehicleSpawnLogic::VIEWING:
			container = &viewingIndices;
			break;
		default:
			return;
	}

	std::unordered_map<VehiclePartType, int>::iterator indexIter = container->find( type );
	if ( container->end() == indexIter )
		container->insert( { type, newIndex } );
	else
		indexIter->second = newIndex;
}

void UVehicleSpawnLogic::replacePart( VehiclePartType type, PartStatus status, std::vector<AActor*> actors, bool destroyExisting )
{
	std::unordered_map<VehiclePartType, std::vector<AActor*>> *container;
	switch ( status )
	{
		case UVehicleSpawnLogic::STANDBY_LEFT:
			container = &standbyPartsLeft;
			break;
		case UVehicleSpawnLogic::SPAWNED:
			container = &spawnedParts;
			break;
		case UVehicleSpawnLogic::STANDBY_RIGHT:
			container = &standbyPartsRight;
			break;
		case UVehicleSpawnLogic::VIEWING:
			container = &viewingParts;
			break;
		default:
			return;
	}

	removePart( type, status, destroyExisting );
	container->insert( { type,actors } );
}

void UVehicleSpawnLogic::spawnPart( VehiclePartType type, PartStatus status, bool destroyExisting )
{
	AActor* owner = GetOwner();
	check( owner );
	USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>( owner->GetComponentByClass( USkeletalMeshComponent::StaticClass() ) );
	check( skeletalMeshComponent );
	UWorld* world = owner->GetWorld();
	check( world );

	std::unordered_map<VehiclePartType, std::vector<UVehiclePart*>>::iterator placeholdersIter = placeholders.find( type );
	// no such type registered in placeholders, no need to spawn this type of vehicle part
	if ( placeholders.end() == placeholdersIter )
		return;
	//////////////////////////////////////////////////////////////////////////
	// should this be checked by whoever that calls spawnPart? it's a private func...
	//////////////////////////////////////////////////////////////////////////

	std::unordered_map<VehiclePartType, int> indicesToUse;
	bool shouldHide = false;
	switch ( status )
	{
		case UVehicleSpawnLogic::STANDBY_LEFT:
			indicesToUse = standbyIndicesLeft;
			shouldHide = true;
			break;
		case UVehicleSpawnLogic::SPAWNED:
			indicesToUse = spawnedIndices;
			break;
		case UVehicleSpawnLogic::STANDBY_RIGHT:
			indicesToUse = standbyIndicesRight;
			shouldHide = true;
			break;
		case UVehicleSpawnLogic::VIEWING:
			indicesToUse = viewingIndices;
			break;
		default:
			// shouldn't come in here!!
			break;
	}

	std::unordered_map<VehiclePartType, int>::iterator indexIter = indicesToUse.find( type );
	if ( indexIter == indicesToUse.end() )
		return;
	//////////////////////////////////////////////////////////////////////////
	// should whoever call spawnPart check for index used to spawn?
	//////////////////////////////////////////////////////////////////////////
	// standby parts may be empty!
	//checkf( indexIter != indicesToUse.end(), *FString( "Indices to spawn from is lacking!" ) );
	int idx = indexIter->second;

	std::unordered_map<VehiclePartType, std::vector<TSubclassOf<AActor>>>::iterator classesIter = spawnableClasses.find( type );
	check( classesIter != spawnableClasses.end() );
	std::vector<TSubclassOf<AActor>> classes = classesIter->second;
	checkf( idx >= 0 && idx < classes.size(), *FString( "Index to spawn is out of bound!" ) );
	TSubclassOf<AActor> classToSpawn = classes[idx];

	if ( nullptr == classToSpawn )
	{
		removePart( type, status, destroyExisting );
	}
	else
	{
		FActorSpawnParameters spawnParam	= FActorSpawnParameters();
		spawnParam.Owner					= owner;
		APawn* ownerPawn					= Cast<APawn>( owner );
		spawnParam.Instigator				= ownerPawn;

		std::vector<UVehiclePart*> placeholdersComponents = placeholdersIter->second;
		std::vector<AActor*> spawnedActors;
		for ( int i = 0; i < placeholdersComponents.size(); i++ )
		{
			UVehiclePart* vehiclePart = placeholdersComponents[i];
			FTransform transform = vehiclePart->GetRelativeTransform();
			AActor* spawnedPart = owner->GetWorld()->SpawnActor<AActor>( classToSpawn, transform, spawnParam );
			spawnedPart->SetActorScale3D( transform.GetScale3D() );
			spawnedPart->AttachToComponent( skeletalMeshComponent, FAttachmentTransformRules( EAttachmentRule::KeepRelative, true ), vehiclePart->SocketName );
			spawnedActors.push_back( spawnedPart );

			if ( isFromMenu )
			{
				UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>( spawnedPart->GetComponentByClass( UStaticMeshComponent::StaticClass() ) );
				staticMeshComp->SetCollisionEnabled( ECollisionEnabled::NoCollision );
			}
		}

		hideActors( spawnedActors, shouldHide );
		replacePart( type, status, spawnedActors, destroyExisting );
	}
}

void UVehicleSpawnLogic::warpValueInRange( int &value, int min, int max )
{
	if ( value < min )
		value = max;
	else if ( value > max )
		value = min;
}
