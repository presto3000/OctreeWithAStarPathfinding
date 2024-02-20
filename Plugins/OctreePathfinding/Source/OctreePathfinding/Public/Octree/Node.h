// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Octree/OctreeActor.h"


class FNodeAStar;

// ---------------- EDGE ------------------ //

struct FEdgeStruct
{
	FNodeAStar* StartNode;
	FNodeAStar* EndNode;
	FEdgeStruct(FNodeAStar* FromNode, FNodeAStar* ToNode)
	{
		StartNode = FromNode;
		EndNode = ToNode;
	}
	~FEdgeStruct();
};

// ---------------- NODE --------------- //
class FNodeAStar
{
	
public:
	FNodeOctree* GetNode () const
	{
		return OctreeNode;
	};
	
	TArray<FEdgeStruct*> EdgeList{};
	FNodeOctree* OctreeNode;

	float F;
	float G;
	float H;
	
	FNodeAStar* CameFrom;
	FNodeAStar();
	FNodeAStar(FNodeOctree* InNode): F(0), G(0), H(0), CameFrom(nullptr)
	{
		OctreeNode = InNode;
	}
	;
	~FNodeAStar();


	
};



