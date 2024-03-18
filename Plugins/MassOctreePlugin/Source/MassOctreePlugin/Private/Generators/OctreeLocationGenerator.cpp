// Fill out your copyright notice in the Description page of Project Settings.


#include "Generators/OctreeLocationGenerator.h"

#include "MassSpawnLocationProcessor.h"
#include "Subsystems/EntityDirectorSubsystem.h"

void UOctreeLocationGenerator::Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes,
                                        int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const
{
	if (Count <= 0)
	{
		FinishedGeneratingSpawnPointsDelegate.Execute(TArray<FMassEntitySpawnDataGeneratorResult>());
		return;
	}

	UEntityDirectorSubsystem* EntityDirectorSubsystem = UWorld::GetSubsystem<UEntityDirectorSubsystem>(QueryOwner.GetWorld());
	if (EntityDirectorSubsystem == nullptr)
	{
		return;
	}
	
	EntityDirectorSubsystem->SetAvailableLocationsToSpawn();
	TArray<FVector> Locations = EntityDirectorSubsystem->AvailableLocationsToSpawn;
	
	if (Locations.IsEmpty())
	{
		return;
	}
	
	// Randomize
	const FRandomStream RandomStream(GetRandomSelectionSeed());
	// Randomize them
	for (int32 I = 0; I < Locations.Num(); ++I)
	{
		const int32 J = RandomStream.RandHelper(Locations.Num());
		Locations.Swap(I, J);
	}
	
	// Build array of entity types to spawn.
	TArray<FMassEntitySpawnDataGeneratorResult> Results;
	BuildResultsFromEntityTypes(Count, EntityTypes, Results);
	
	const int32 LocationCount = Locations.Num();
	int32 LocationIndex = 0;

	// Distribute points amongst the entities to spawn.
	for (FMassEntitySpawnDataGeneratorResult& Result : Results)
	{
		Result.SpawnDataProcessor = UMassSpawnLocationProcessor::StaticClass();
		Result.SpawnData.InitializeAs<FMassTransformsSpawnData>();
		FMassTransformsSpawnData& Transforms = Result.SpawnData.GetMutable<FMassTransformsSpawnData>();

		Transforms.Transforms.Reserve(Result.NumEntities);
		for (int i = 0; i < Result.NumEntities; i++)
		{
			FTransform& Transform = Transforms.Transforms.AddDefaulted_GetRef();
			Transform.SetLocation(Locations[LocationIndex % LocationCount]);
			LocationIndex++;
		}
	}
	
	FinishedGeneratingSpawnPointsDelegate.Execute(Results);
}
