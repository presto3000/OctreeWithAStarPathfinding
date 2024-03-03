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

UENUM(BlueprintType)
enum class EFlyingPawnState : uint8
{
	Waiting,
	MoveToTarget,
	DoingJob,
	Resting,
	Attacking
};

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
	
	UFUNCTION(BlueprintCallable)
	void FindRandomDestinationAndPath();
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flying Properties", meta=(AllowPrivateAccess="true"))
	bool bIsMoving = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flying Properties")
	bool bHasReachedDestination = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Properties")
	EFlyingPawnState FlyingPawnState = EFlyingPawnState::Waiting;
	
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

private:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	USceneComponent* RootComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

	
public:
	virtual void Tick(float DeltaTime) override;
	
	FORCEINLINE UBehaviorTree* GetBehaviourTree() const {return Tree; }

	FORCEINLINE UStaticMeshComponent* GetMesh() const {return Mesh; }
	
	FORCEINLINE void SetFlyingPawnState(EFlyingPawnState NewFlyingPawnState)  { FlyingPawnState = NewFlyingPawnState; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsMoving() const { return bIsMoving; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsMoving (const bool NewIsMoving) { bIsMoving = NewIsMoving; }
	
};
