// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "MyFlyingAIController.generated.h"

UCLASS()
class OCTREEPATHFINDING_API AMyFlyingAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyFlyingAIController();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
public:

	virtual void Tick(float DeltaTime) override;
};
