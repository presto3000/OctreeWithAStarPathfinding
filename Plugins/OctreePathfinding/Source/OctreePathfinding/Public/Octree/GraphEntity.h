// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Node.h"
#include "Octree/OctreeActor.h"
#include "GameFramework/Actor.h"

// ---------------- GRAPH ------------------ //

class FGraphEntity
{

public:
	FGraphEntity();
	~FGraphEntity();
	
	void AddNode(FNodeOctree& Otn);
	void Draw(UWorld* InWorldContext);
	
	FNodeAStar* FindNode(const int32 OctreeNodeId);
	void AddEdge(const FNodeOctree* FromNode, const FNodeOctree* ToNode);
	bool AStar(const FNodeOctree* StartNode, const FNodeOctree* EndNode, TArray<FNodeAStar*>& PathList);
	int32 LowestF(TArray<FNodeAStar*> InNodes);
	void ReconstructPath(FNodeAStar* StartNode, FNodeAStar* EndNode, TArray<FNodeAStar*>& PathList);

	TArray<FEdgeStruct*> Edges {};
	TArray<FNodeAStar*> Nodes {};

	FCriticalSection AddEdgeCriticalSection2;

};