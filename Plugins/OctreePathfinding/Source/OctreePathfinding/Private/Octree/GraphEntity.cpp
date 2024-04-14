// Fill out your copyright notice in the Description page of Project Settings.


#include "Octree/GraphEntity.h"


FGraphEntity::FGraphEntity()
{
	
}

FGraphEntity::~FGraphEntity()
{
	//PrestoLOG::Log("FGraphEntity Destructor");

	// Free them
	for (const FEdgeStruct* Edge : Edges)
	{
		//PrestoLOG::Log("Edge Destructor");
		delete Edge;
	}
	Edges.Empty();

	for (const FNodeAStar* Node : Nodes)
	{
		//PrestoLOG::Log("Node Destructor");
		delete Node;
	}
	Nodes.Empty();
}

void FGraphEntity::AddNode(FNodeOctree& Otn)
{
	if (FindNode(Otn.Id) == nullptr)
	{
		FNodeAStar* NewNode = new FNodeAStar(&Otn); // Creating a new FNode with a pointer to the FNodeOctree
		Nodes.Add(NewNode); 

	}
}

FNodeAStar* FGraphEntity::FindNode(const int32 OctreeNodeId)
{
	for (const auto& Node : Nodes)
	{
		if (Node->GetNode()->Id == OctreeNodeId)
		{
			return Node;
		}
	}
	return nullptr;
}

void FGraphEntity::AddEdge(const FNodeOctree* FromNode, const FNodeOctree* ToNode)
{
	FScopeLock Lock(&AddEdgeCriticalSection2);
	if (!FromNode || !ToNode)
	{
		return;
	}

	FNodeAStar* From = FindNode(FromNode->Id);
	FNodeAStar* To = FindNode(ToNode->Id);

	if (From && To)
	{
		// Check if the edge already exists
		bool EdgeExists = false;
		for (FEdgeStruct* ExistingEdge : From->EdgeList)
		{
			if (ExistingEdge->EndNode == To)
			{
				EdgeExists = true;
				break;
			}
		}

		if (!EdgeExists)
		{
			// One Direction
			FEdgeStruct* E = new FEdgeStruct(From, To);
			Edges.Add(E);
			From->EdgeList.Add(E);
			// 2nd Direction
			FEdgeStruct* F = new FEdgeStruct(To, From);
			Edges.Add(F);
			To->EdgeList.Add(F);
		}
	}
}

void FGraphEntity::RemoveInvalidEdges()
{
	TArray<FEdgeStruct*> InvalidEdges;
	
	for (FEdgeStruct* Edge : Edges)
	{
		if (!Edge->StartNode->OctreeNode->IsLeafNode() || !Edge->EndNode->OctreeNode->IsLeafNode() || Edge->StartNode->OctreeNode->bIsOccupied || Edge->EndNode->OctreeNode->bIsOccupied)
		{
			InvalidEdges.Add(Edge);
		}
	}
	
	for (FEdgeStruct* InvalidEdge : InvalidEdges)
	{
		InvalidEdge->StartNode->EdgeList.Remove(InvalidEdge);
		InvalidEdge->EndNode->EdgeList.Remove(InvalidEdge);
		Edges.Remove(InvalidEdge);
		delete InvalidEdge;
	}
}

void FGraphEntity::RemoveInvalidNodes()
{
	TArray<FNodeAStar*> InvalidNodes;
	
	for (FNodeAStar* Node : Nodes)
	{
		if (!Node->OctreeNode->IsLeafNode() || Node->OctreeNode->bIsOccupied)
		{
			InvalidNodes.Add(Node);
		}
	}
	
	for (FNodeAStar* InvalidNode : InvalidNodes)
	{
		for (FEdgeStruct* Edge : InvalidNode->EdgeList)
		{
			Edges.Remove(Edge);
			delete Edge;
		}
		InvalidNode->EdgeList.Empty();
		
		Nodes.Remove(InvalidNode);
		delete InvalidNode;
	}
}

bool FGraphEntity::AStar(const FNodeOctree* StartNode, const FNodeOctree* EndNode, TArray<FNodeAStar*>& PathList)
{
	PathList.Empty();
	FNodeAStar* Start = FindNode(StartNode->Id);
	FNodeAStar* End = FindNode(EndNode->Id);

	if (Start == nullptr || End == nullptr)
	{
		return false;
	}

	TArray<FNodeAStar*> Open;
	TArray<FNodeAStar*> Closed;

	bool bTentativeIsBetter;
	
	Start->G = 0;
	Start->H = FVector{StartNode->Center - EndNode->Center}.SizeSquared();
	Start->F = Start->H;
	
	Open.Add(Start);

	while (Open.Num() > 0)
	{
		// 1. lowest cost
		const int32 i = LowestF(Open);

		FNodeAStar* ThisNode = Open[i];
		if (ThisNode->OctreeNode->Id == EndNode->Id)
		{
			ReconstructPath(Start, End, PathList);
			return true;
		}
		Open.RemoveAt(i);
		
		if (ThisNode && ThisNode->CameFrom)
		{
			Closed.Add(ThisNode);
		}

		FNodeAStar* Neighbour;
		for (const auto& Edge : ThisNode->EdgeList)
		{
			Neighbour = Edge->EndNode;
			if (Neighbour && Neighbour->OctreeNode != nullptr)
			{
				// Distance between Node and a Neighbour
				Neighbour->G = ThisNode->G + FVector{ThisNode->OctreeNode->Center - Neighbour->OctreeNode->Center}.SizeSquared();

				// Neighbour in Closed 
				if (Closed.Contains(Neighbour))
				{
					continue;
				}
				const float TentativeGScore = ThisNode->G + FVector{ThisNode->OctreeNode->Center - Neighbour->OctreeNode->Center}.
					SizeSquared();
				if (!Open.Contains(Neighbour))
				{
					Open.Add(Neighbour);
					bTentativeIsBetter = true;
				}
				else if (TentativeGScore < Neighbour->G)
				{
					bTentativeIsBetter = true;
				}
				else
				{
					bTentativeIsBetter = false;
				}

				if (bTentativeIsBetter)
				{
					Neighbour->CameFrom = ThisNode;
					Neighbour->G = TentativeGScore;
					Neighbour->H = FVector{ThisNode->OctreeNode->Center - EndNode->Center}.SizeSquared();

					Neighbour->F = Neighbour->G + Neighbour->H;
				}
			}
			
		}
	}
	return false;
}

int32 FGraphEntity::LowestF(TArray<FNodeAStar*> InNodes)
{
	float LowestF = 0;
	int32 Count = 0;
	int IteratorCount = 0;

	for (int32 i = 0; i < InNodes.Num(); ++i)
	{
		if (i == 0)
		{
			LowestF = InNodes[i]->F;
			IteratorCount = Count;
		}
		else if (InNodes[i]->F <= LowestF)
		{
			LowestF = InNodes[i]->F;
			IteratorCount = Count;
		}
		Count++;
	}
	return IteratorCount;
}

void FGraphEntity::ReconstructPath(FNodeAStar* StartNode, FNodeAStar* EndNode, TArray<FNodeAStar*>& PathList)
{
	PathList.Empty();
	PathList.Add(EndNode);
	FNodeAStar* P = EndNode->CameFrom;
	int32 PathLength = 1;

	while (P != nullptr && P != StartNode && PathLength <= 1000)
	{
		PathList.Insert(P, 0); // Insert at the beginning to maintain the correct order
		P = P->CameFrom;

		++PathLength;
	}

	if (PathLength > 1000 || P == nullptr)
	{
		// If the path list becomes too large or P becomes nullptr
		PathList.Empty();
		return;
	}

	PathList.Insert(StartNode, 0); // Insert the start node at the beginning
}


void FGraphEntity::Draw(UWorld* InWorldContext)
{
	for (int32 i = 0; i < Edges.Num(); ++i)
	{
		if (Edges[i])
		{
			if (Edges[i]->StartNode && Edges[i]->StartNode->OctreeNode && Edges[i]->EndNode && Edges[i]->EndNode->OctreeNode)
			{
				if (Edges[i]->StartNode->OctreeNode->Center.IsNearlyZero(1) || Edges[i]->EndNode->OctreeNode->Center.IsNearlyZero(1))
				{
					//PrestoLOG::ScreenLog5("NEAR ZERO");
					continue;
				}
				DrawDebugLine(InWorldContext, Edges[i]->StartNode->OctreeNode->Center, Edges[i]->EndNode->OctreeNode->Center, FColor::Red, true, -1, 0, 3);
			}
		}
	}
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		DrawDebugSphere(InWorldContext, Nodes[i]->OctreeNode->NodeBounds.GetCenter(), Nodes[i]->OctreeNode->NodeBounds.GetExtent().Length()/2.f, 12, FColor::Yellow, true, -10, 0);
	}
}


