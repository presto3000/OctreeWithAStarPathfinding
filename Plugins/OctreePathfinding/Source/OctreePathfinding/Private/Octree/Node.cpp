// Fill out your copyright notice in the Description page of Project Settings.


#include "Octree/Node.h"

FEdgeStruct::~FEdgeStruct()
{
	PrestoLOG::Log("FEdgeStruct Destructor");
}

FNodeAStar::FNodeAStar()
{
	
}

FNodeAStar::~FNodeAStar()
{
	PrestoLOG::Log("FNodeAStar Destructor");

	// Free them
	for (const FEdgeStruct* Edge : EdgeList)
	{
		delete Edge;
	}
	EdgeList.Empty();
}
