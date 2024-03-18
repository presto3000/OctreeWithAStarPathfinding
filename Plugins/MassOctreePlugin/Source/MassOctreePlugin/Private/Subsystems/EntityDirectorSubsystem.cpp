// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EntityDirectorSubsystem.h"

#include "MassActorSubsystem.h"
#include "MassEntitySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Octree/OctreeActor.h"

void UEntityDirectorSubsystem::DestroyEntityActor(const AActor* InActor)
{
	UMassActorSubsystem* ActorSubsystem = UWorld::GetSubsystem<UMassActorSubsystem>(GetWorld());
	if (ActorSubsystem)
	{
		const FMassEntityHandle InEntityHandle = ActorSubsystem->GetEntityHandleFromActor(InActor);
		if (const UMassEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UMassEntitySubsystem>(GetWorld()))
		{
			const FMassEntityManager& EntityManager = EntitySubsystem->GetEntityManager();
			EntityManager.Defer().DestroyEntity(InEntityHandle);
		}
	}
}

TArray<FVector> UEntityDirectorSubsystem::SetAvailableLocationsToSpawn()
{
	AOctreeActor* OctreeActorRef = Cast<AOctreeActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AOctreeActor::StaticClass()));
	if (OctreeActorRef)
	{
		for (const auto& EmptyLeaf : OctreeActorRef->EmptyLeaves)
		{
			AvailableLocationsToSpawn.AddUnique(EmptyLeaf->Center);
		}
	}
	return AvailableLocationsToSpawn;
}
