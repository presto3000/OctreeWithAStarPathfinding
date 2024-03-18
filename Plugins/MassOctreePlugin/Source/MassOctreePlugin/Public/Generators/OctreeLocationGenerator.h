// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntitySpawnDataGeneratorBase.h"

#include "OctreeLocationGenerator.generated.h"

/**
 * 
 */
UCLASS()
class MASSOCTREEPLUGIN_API UOctreeLocationGenerator : public UMassEntitySpawnDataGeneratorBase
{
	GENERATED_BODY()

	virtual void Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes, int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const override;
	
};
