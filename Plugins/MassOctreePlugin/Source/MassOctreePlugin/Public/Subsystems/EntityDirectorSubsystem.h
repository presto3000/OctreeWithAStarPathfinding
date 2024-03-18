// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EntityDirectorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MASSOCTREEPLUGIN_API UEntityDirectorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "DirectorSubsystem")
	void DestroyEntityActor(const AActor* InActor);

	// Empty Leaves of Octree in this case
	TArray<FVector> SetAvailableLocationsToSpawn();
	TArray<FVector> AvailableLocationsToSpawn;
};
