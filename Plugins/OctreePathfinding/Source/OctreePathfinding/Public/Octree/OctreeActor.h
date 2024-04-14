// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Octree/Utils.h"
#include "Runnables/EdgesRunnable.h"
#include "OctreeActor.generated.h"

class ALandscapeProxy;
class FGraphEntity;
class UBoxComponent;

// ---------------- OCTREE NODE --------------- //

USTRUCT(BlueprintType)
struct FNodeOctree
{
	GENERATED_USTRUCT_BODY()
	int32 Id;
	FBox NodeBounds;
	TArray<FBox> ChildBounds;
	TArray<FNodeOctree*> Children;
	float MinSize = 100;
	UPROPERTY()
	TArray<AActor*> ContainedActors;
	FNodeOctree* Parent;
	FVector Center;
	FVector HalfSize;
	bool bIsOccupied = false;
	
	void Draw(UWorld* InWorldContext);
	void DivideAndAdd(UWorld* InWorldContext, AActor* InActor);
	void AddObject(UWorld* InWorldContext, AActor* InObject);
	void MarkNodesAsNotOccupied(const AActor* InActor);
	void RemoveAllOctreeNodes();
	
	bool HasActors() const
	{
		return ContainedActors.Num() > 0;
	}

	bool IsLeafNode() const
	{
		return Children.Num() == 0;
	}

	void RemoveActor(AActor* ActorToRemove)
	{
		ContainedActors.Remove(ActorToRemove);
	}

	bool ContainsActor(AActor* ActorToCheck) const
	{
		return ContainedActors.Contains(ActorToCheck);
	}

	
	FNodeOctree(): Id(0), Parent(nullptr)
	{
	} ;
	FNodeOctree(FBox InBounds, float InMinSize, FNodeOctree* InParent):
	NodeBounds(InBounds),
	MinSize(InMinSize),
	Center(NodeBounds.GetCenter()),
	Parent(InParent)
	{
		Id = FUtils::Id_Number++;

		ChildBounds.Init(FBox(), 8);
		
		// Calculate child bounds and assign them to ChildBounds
		FVector Min = NodeBounds.Min;
		FVector Max = NodeBounds.Max;
		HalfSize = NodeBounds.GetSize() * 0.5f;

		// Lower bounds
		FVector Lower = Min;
		// Upper bounds
		FVector Upper = Max;
		
		ChildBounds[0] = FBox(Lower, Lower + HalfSize);
		ChildBounds[1] = FBox(FVector(Center.X, Lower.Y, Lower.Z), FVector(Upper.X, Center.Y, Center.Z));
		ChildBounds[2] = FBox(FVector(Lower.X, Center.Y, Lower.Z), FVector(Center.X, Upper.Y, Center.Z));
		ChildBounds[3] = FBox(FVector(Center.X, Center.Y, Lower.Z), FVector(Upper.X, Upper.Y, Center.Z));
		ChildBounds[4] = FBox(FVector(Lower.X, Lower.Y, Center.Z), FVector(Center.X, Center.Y, Upper.Z));
		ChildBounds[5] = FBox(FVector(Center.X, Lower.Y, Center.Z), FVector(Upper.X, Center.Y, Upper.Z));
		ChildBounds[6] = FBox(FVector(Lower.X, Center.Y, Center.Z), FVector(Center.X, Upper.Y, Upper.Z));
		ChildBounds[7] = FBox(Center, Upper);
	}
	~FNodeOctree();
};

// ---------------- OCTREE ACTOR --------------- //
UCLASS()
class OCTREEPATHFINDING_API AOctreeActor : public AActor
{
	GENERATED_BODY()

public:
	AOctreeActor();
	
	FGraphEntity* NavigationGraph = nullptr;
	int32 AddDestination(const FVector& Destination);
	
	UPROPERTY(EditAnywhere, Category = DEBUG)
	bool bShowDebug = true;
	UPROPERTY(EditAnywhere, Category = DEBUG)
	bool bDebugLines = true;
	UPROPERTY(EditAnywhere, Category = DEBUG)
	bool bDebugLinesGraph = true;
	UPROPERTY(EditAnywhere, Category = DEBUG)
	bool bAddLandscapeActor = true;
	UPROPERTY(EditAnywhere, Category = DEBUG)
	bool bProcessObjectsAndEdgesOnStart = true;
	
	TArray<FNodeOctree*> EmptyLeaves;
	bool IntersectRayWithBox(const FVector& RayOrigin, const FVector& RayDirection, const FBox& Box, float& HitLength, float MaxLength);
	
protected:
	
	virtual void BeginPlay() override;

	FNodeOctree* RootNode;

	UPROPERTY()
	ALandscapeProxy* LandscapeActoro;

	TArray<TUniquePtr<EdgesRunnable>> EdgesRunnables;
	TArray<FRunnableThread*> Threads;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* RootComp;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UBoxComponent* BoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*> WorldActors;
	UPROPERTY(EditAnywhere)
	float MinNodeSize = 100.f;


	// Declaration of a list to store nodes to delete
	TArray<FNodeOctree*> NodesToDelete;
	
	UFUNCTION(BlueprintCallable)
	void CreateOctree();
	UFUNCTION(CallInEditor)
	void GetActorsInsideBoxComponent();
	UFUNCTION(BlueprintCallable)
	void AddObjects(TArray<AActor*> InWorldActors);

	int32 FindBindingNode(FNodeOctree* Node, const FVector& Position);
	void GetEmptyLeaves(FNodeOctree* InNode);
	void ProcessExtraConnections();
	void ConnectLeafNodeNeighbours();
	
	void ConnectLeafNodeNeighboursMultithreaded();
	void ProcessLeafNodes(int startIndex, int endIndex);

	UFUNCTION(BlueprintCallable)
	void DeleteAllNodesFromOctree();
	UFUNCTION(BlueprintCallable)
	void MarkNodesAsUnOccupiedForActor(const AActor* InActor) const;
	UFUNCTION(BlueprintCallable)
	void RemoveActorFromOctreeBP(AActor* ActorToRemove);
	
	void RemoveActorFromOctree(AActor* ActorToRemove, FNodeOctree*& CurrentNode);
	void PurgeEmptyBranches(TArray<FNodeOctree*>& InOctreeNodes);
	void PurgeBranchIfEmpty(FNodeOctree*& Node);

	
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void DrawDebug();
	void DrawEmptyLeaves();

	UFUNCTION(BlueprintCallable)
	void ProcessObjectsAndEdges(const TArray<AActor*>& InWorldActors);

public:
	virtual void Tick(float DeltaTime) override;

};
