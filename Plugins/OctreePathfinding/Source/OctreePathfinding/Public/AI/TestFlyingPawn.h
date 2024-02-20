// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TestFlyingPawn.generated.h"

class FNodeAStar;
class AGoalActor;
class FGraphEntity;
class AOctreeActor;
struct FNodeOctree;
class UBehaviorTree;

UCLASS()
class OCTREEPATHFINDING_API ATestFlyingPawn : public APawn
{
	GENERATED_BODY()

public:
	ATestFlyingPawn();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Navigate();

	void NavigateTo(const int32 StartId, const int32 DestinationId, FNodeAStar* FinalGoal);
	void GetRandomDestination();
	int32 GetPathLength() const;
	FNodeOctree* GetPathPoint(const int32 InIndex);
	void Update(AGoalActor* InGoalActor);
	void UpdateMovement(float DeltaTime);
	void UpdateMovement2(float DeltaTime);
	void DebugPath(float InDuration, bool InDrawPoints);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	UBehaviorTree* Tree;

	UPROPERTY(EditAnywhere, Category = "Flying Properties")
	float Speed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Flying Properties")
	float Accuracy = 10.f;

	UPROPERTY(EditAnywhere, Category = "Flying Properties")
	float RotSpeed = 5.f;

	// Current Waypoint
	int32 CurrentWP = 0;
	
	FVector Goal;
	FNodeOctree* CurrentNode;
	
	UPROPERTY()
	AOctreeActor* OctreeActor;
	FGraphEntity* Graph = nullptr;
	TArray<FNodeAStar*> PathList {};

	UPROPERTY()
	AGoalActor* GoalActor;
	FVector GoalPosition;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	FORCEINLINE UBehaviorTree* GetBehaviourTree() const {return Tree; }
};
