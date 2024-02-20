// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoalActor.generated.h"

UCLASS()
class OCTREEPATHFINDING_API AGoalActor : public AActor
{
	GENERATED_BODY()

public:
	AGoalActor();

protected:
	virtual void BeginPlay() override;

public:
};
