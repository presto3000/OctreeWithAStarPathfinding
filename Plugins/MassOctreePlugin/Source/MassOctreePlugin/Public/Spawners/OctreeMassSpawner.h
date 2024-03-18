// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSpawner.h"
#include "OctreeMassSpawner.generated.h"

UCLASS()
class MASSOCTREEPLUGIN_API AOctreeMassSpawner : public AMassSpawner
{
	GENERATED_BODY()

public:
	AOctreeMassSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
