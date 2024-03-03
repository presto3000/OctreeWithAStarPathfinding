// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"

#include "FlyingStateTreeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OCTREEPATHFINDING_API UFlyingStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	UFlyingStateTreeComponent();

protected:
	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "StateTree")
	void SetStateTree(UStateTree* InStateTree);
};
