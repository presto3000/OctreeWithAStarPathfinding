// Fill out your copyright notice in the Description page of Project Settings.


#include "Octree/OctreeActor.h"

#include <thread>


#include "LandscapeProxy.h"
#include "Actors/TestCube.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Octree/GraphEntity.h"
#include "Runnables/EdgesRunnable.h"

FNodeOctree::~FNodeOctree()
{
	// Delete child nodes
	for (int32 i = Children.Num() - 1; i >= 0; --i)
	{
		if (Children[i] != nullptr)
		{
			delete Children[i];
			Children[i] = nullptr;
		}
	}
	Children.Empty();
	
	PrestoLOG::Log("FNode OctreeDeleted: ", this->Id);
}

void FNodeOctree::Draw(UWorld* InWorldContext)
{
	DrawDebugBox(InWorldContext, NodeBounds.GetCenter(), NodeBounds.GetExtent(), FColor::Green, true, -1, 0, 3);
	//for (const auto& ChildBound : ChildBounds)
	//{
	//	DrawDebugBox(InWorldContext, ChildBound.GetCenter(), ChildBound.GetExtent(), FColor::Red, true, -1, 0, 3);
	//}
	//for (const auto& ContainedActor : ContainedActors)
	//{
	//	DrawDebugBox(InWorldContext, ContainedActor->Bounds.GetCenter(), ContainedActor->Bounds.GetExtent(), FColor::Red, true, -1, 0, 3);
	//}

	
	// Another recursion
	if (!Children.IsEmpty())
	{
		for (int32 i = 0; i < 8; ++i)
		{
			if (Children[i] != nullptr)
			{
				Children[i]->Draw(InWorldContext);
			}
		}
	}
	//else if (ContainedActors.IsEmpty())
	//{
	//	//DrawDebugSphere(InWorldContext, NodeBounds.GetCenter(), NodeBounds.GetExtent().Length()/2.f, 12, FColor::Blue, true, -10, 0);
	//}
	// Draw Occupied
	if (bIsOccupied)
	{
		DrawDebugSphere(InWorldContext, NodeBounds.GetCenter(), NodeBounds.GetExtent().Length()/2.f, 12, FColor::Blue, true, -10, 0);
	}
}

void FNodeOctree::MarkNodesAsNotOccupied(const AActor* InActor)
{
	if (NodeBounds.Intersect(InActor->GetComponentsBoundingBox()))
	{
		bIsOccupied = false;
		// Recursively mark child nodes as not occupied
		for (int32 i = 0; i < 8; ++i)
		{
			if (!IsLeafNode())
			{
				if (Children[i] != nullptr)
				{
					Children[i]->MarkNodesAsNotOccupied(InActor);
				}
			}
		}
	}
}

void FNodeOctree::RemoveAllOctreeNodes()
{
	for (FNodeOctree* ChildNode : Children)
	{
		if (ChildNode != nullptr)
		{
			ChildNode->RemoveAllOctreeNodes();
			//  Delete child nodes
			delete ChildNode;
			ChildNode = nullptr;
		}
	}
	Children.Empty();
}


void AOctreeActor::BeginPlay()
{
	Super::BeginPlay();

}
void AOctreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOctreeActor::CreateOctree()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	//DrawDebugBox(GetWorld(), BoxComponent->GetComponentLocation(), BoxComponent->GetScaledBoxExtent(), FColor::Green, true, -1, 0, 3);
	LandscapeActoro = Cast<ALandscapeProxy>(UGameplayStatics::GetActorOfClass(World, ALandscapeProxy::StaticClass()));
	if (LandscapeActoro)
	{
		PrestoLOG::Log(LandscapeActoro->GetName());
	}
	
	if (bAddLandscapeActor)
	{
		WorldActors.Add(LandscapeActoro);
	}
	GetActorsInsideBoxComponent();


	NavigationGraph = new FGraphEntity();
	if (NavigationGraph)
	{
		// Get the local bounds of the box component
		const FBoxSphereBounds LocalBounds = BoxComponent->GetLocalBounds();
		// Get the FBox part of the bounds
		const FBox BoxPart = LocalBounds.GetBox();
		// Transform the box bounds to world space using actor's location
		const FTransform ActorTransform = GetActorTransform();
		const FBox WorldBoxBounds = BoxPart.TransformBy(ActorTransform);
	
		RootNode = new FNodeOctree(WorldBoxBounds, MinNodeSize, nullptr);
		const double Start = FPlatformTime::Seconds();
		AddObjects(WorldActors);
		const double End = FPlatformTime::Seconds();
		UE_LOG(LogTemp, Warning, TEXT("OCTREE CRATED in: %f ms"),(End-Start)*1000.f);

		if (bProcessObjectsAndEdgesOnStart)
		{
			ProcessObjectsAndEdges(WorldActors);
		}
		
	}

}

void AOctreeActor::ProcessObjectsAndEdges(const TArray<AActor*>& InWorldActors)
{
	GetEmptyLeaves(RootNode);
	//DrawEmptyLeaves();
	ConnectLeafNodeNeighboursMultithreaded();

	
	PrestoLOG::Log("RootNodeId:", RootNode->Id);
	PrestoLOG::Log("Nodes Number: ", NavigationGraph->Nodes.Num());
	PrestoLOG::Log("Edges Number: ", NavigationGraph->Edges.Num());
}

void AOctreeActor::DrawDebug()
{
	if (bDebugLines)
	{
		RootNode->Draw(GetWorld());
			
	}
	if (bDebugLinesGraph)
	{
		NavigationGraph->Draw(GetWorld());
	}
}

void AOctreeActor::DrawEmptyLeaves()
{
	for (int i = 0; i < EmptyLeaves.Num() - 1; ++i)
	{
		DrawDebugPoint(GetWorld(), EmptyLeaves[i]->Center, 40, FColor::Emerald, true, -1);
	}
}

void AOctreeActor::AddObjects(TArray<AActor*> InWorldActors)
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (const auto& WorldActor : InWorldActors)
		{
			RootNode->AddObject(World, WorldActor);
		}
	}

}
void FNodeOctree::AddObject(UWorld* InWorldContext, AActor* InObject)
{
	DivideAndAdd(InWorldContext, InObject);
}
void FNodeOctree::DivideAndAdd(UWorld* InWorldContext, AActor* InActor)
{	
	// Stop Condition
	if (NodeBounds.GetSize().Y <= MinSize)
	{
		ContainedActors.Add(InActor);
		bIsOccupied = true;
		//DrawDebugBox(InWorldContext, NodeBounds.GetCenter(), NodeBounds.GetExtent(), FColor::Cyan, true, -1, 0, 7);
		return;
	}

	if (Children.IsEmpty())
	{
		Children.SetNum(8);
	}

	bool bDividing = false;

	//DrawDebugPoint(InWorldContext, Center, 40, FColor::White, true, -1);
	for (int32 i = 0; i < 8; ++i)
	{
		bool bCenterIsBelowLandscape = false;
		if (Children[i] == nullptr)
		{
			Children[i] = new FNodeOctree(ChildBounds[i], MinSize, this);
		}
		
		FCollisionQueryParams Params;
		Params.bTraceComplex = true;
		Params.bReturnPhysicalMaterial = false;
		FHitResult HitResult;
		FVector Start = ChildBounds[i].GetCenter() + FVector(0, 0, 5000.f); // Start at the top of the node's bounding box
		FVector End = ChildBounds[i].GetCenter() - FVector(0, 0, 5000.f); // End at the bottom of the node's bounding box
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
					 InWorldContext, Start, End,2, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),true,
					 TArray<AActor*>(),  EDrawDebugTrace::None, HitResult, true, FColor::Cyan,
					 FColor::Black, 30 );
		if (bHit)
		{
			if (ChildBounds[i].GetCenter().Z < HitResult.ImpactPoint.Z)
			{
				bDividing = true;
				Children[i]->DivideAndAdd(InWorldContext, InActor);
			}
		}
		if (!bCenterIsBelowLandscape)
		{
			if (InActor->Tags.Contains("Landscape"))
			{
				FHitResult HitResult2;
				FVector Start2 = ChildBounds[i].GetCenter() + FVector(0, 0, ChildBounds[i].GetExtent().Z); // Start at the top of the node's bounding box
				FVector End2 = ChildBounds[i].GetCenter() - FVector(0, 0, ChildBounds[i].GetExtent().Z); // End at the bottom of the node's bounding box
				if (InWorldContext->LineTraceSingleByChannel(HitResult2, Start2, End2, ECC_WorldStatic, Params))
				{
					if (HitResult2.GetActor() == InActor)
					{
						bDividing = true;

						Children[i]->DivideAndAdd(InWorldContext, InActor);
					}
					//DrawDebugLine(InWorldContext, Start, End, FColor::Red, true, -1, 0, 10);
					//PrestoLOG::ScreenLog5("FOUND LANDSCAPE");
				}
			}
			else
			{
				// Is inside / or Intersect
				//if (ChildBounds[i].IsInside(OctObj.Bounds.Min) && ChildBounds[i].IsInside(OctObj.Bounds.Max))
				if (ChildBounds[i].Intersect(InActor->GetComponentsBoundingBox()))
				{
					bDividing = true;
					Children[i]->DivideAndAdd(InWorldContext, InActor);
					//PrestoLOG::ScreenLog5("IS INSIDE");
				}	
			}
		}
	}
	if (bDividing == false)
	{
		ContainedActors.AddUnique(InActor);
		for (FNodeOctree*& ChildNode : Children)
		{
			delete ChildNode;
			ChildNode = nullptr;
		}
	}
}


AOctreeActor::AOctreeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComp);
}


void AOctreeActor::GetActorsInsideBoxComponent()
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	
	UWorld* World = GetWorld();
	if (World) {
		TArray<AActor*> OverlappingActors;
		if (BoxComponent)
		{
			BoxComponent->GetOverlappingActors(OverlappingActors, ATestCube::StaticClass());
			WorldActors.Append(OverlappingActors);
		}
		int32 i = 0;
		for (AActor* OverlappingActor : OverlappingActors)
		{
			++i;
			// Do whatever you need with the overlapping actors
			PrestoLOG::ScreenLog5(i);
		}
	}
}

int32 AOctreeActor::AddDestination(const FVector& Destination)
{
	return FindBindingNode(RootNode, Destination);
}

int32 AOctreeActor::FindBindingNode(FNodeOctree* Node, const FVector& Position)
{
	int32 Found = -1;
	if (Node == nullptr)
	{
		return -1;
	}
	// Leaf node 
	if (Node->Children.Num() == 0)
	{
		if (Node->NodeBounds.IsInsideOrOn(Position) && !Node->bIsOccupied)
		{
			return Node->Id;
		}
	}
	else
	{
		for (int32 i = 0; i < 8; ++i)
		{
			Found = FindBindingNode(Node->Children[i], Position);
			if (Found != -1)
			{
				break;
			}
		}
		return Found;
	}
	return -1;
}

void AOctreeActor::GetEmptyLeaves(FNodeOctree* InNode)
{
	if (InNode == nullptr)
	{
		return;
	}
	// Must be a leaf node
	if (InNode->Children.IsEmpty())
	{
		if (!InNode->bIsOccupied)
		{
			EmptyLeaves.Add(InNode);
			NavigationGraph->AddNode(*InNode);
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			GetEmptyLeaves(InNode->Children[i]);

			// Getting the sibling of any child
		
			//for (int s = 0; s < 8; ++s)
			//{
			//	if (s != i)
			//	{
			//		NavigationGraph->AddEdge(InNode->Children[i], InNode->Children[s]);
			//	}
			//}
		
		}
	}
}

void AOctreeActor::ProcessExtraConnections()
{
	UWorld* World = GetWorld();
	TMap<int32, int32> SubGraphConnections{};
	for (const FNodeOctree* i : EmptyLeaves)
	{
		for (const FNodeOctree* j : EmptyLeaves)
		{
			if (i->Id != j->Id && i->Parent->Id != j->Parent->Id)
			{
				if (World)
				{
					FHitResult HitResult;
					FVector StartLocation = i->Center;
					FVector Direction = j->Center - i->Center;
					float Accuracy = 1;
					FVector EndLocation = StartLocation + Direction * Accuracy;
					bool bHit = UKismetSystemLibrary::SphereTraceSingle(
	  					GetWorld(), 
	  					StartLocation,
	  					EndLocation,
	  					2, 
	  					UEngineTypes::ConvertToTraceType(ECC_Visibility), 
	  					false, 
	  					TArray<AActor*>(), 
	  					EDrawDebugTrace::None, 
	  					HitResult,
	  					true, FColor::Cyan, FColor::Black, 30 
					  );
					if (!bHit)
					{
						if (!SubGraphConnections.Contains(i->Parent->Id))
						{
							SubGraphConnections.Add(i->Parent->Id, j->Parent->Id);
							NavigationGraph->AddEdge(i, j);
						}

					}
				}

			}
		}
	}
}

void AOctreeActor::ConnectLeafNodeNeighbours()
{
    TArray<FVector> Rays;
    Rays.Add(FVector(1.f, 0.f, 0.f));
    Rays.Add(FVector(-1.f, 0.f, 0.f));
    Rays.Add(FVector(0.f, 1.f, 0.f));
    Rays.Add(FVector(0.f, -1.f, 0.f));
    Rays.Add(FVector(1.f, 0.f, 1.f));
    Rays.Add(FVector(1.f, 0.f, -1.f));

	for (int32 i = 0; i < EmptyLeaves.Num(); ++i)
	{
		TArray<FNodeOctree*> Neighbours;
		for (int32 j = 0; j < EmptyLeaves.Num(); ++j)
		{
			if (i != j)
			{
				FVector Start = EmptyLeaves[i]->Center;
				float Distance = FVector::Distance(Start, EmptyLeaves[j]->Center);
				
				float MaxSizeThreshold = EmptyLeaves[i]->NodeBounds.GetExtent().Size() + EmptyLeaves[j]->NodeBounds.GetExtent().Size();
				if (Distance < MaxSizeThreshold)
				{
					for (int32 r = 0; r < 6; ++r)
					{
						FVector Direction = Rays[r];
						float MaxLength = EmptyLeaves[i]->NodeBounds.GetExtent().Size() + 0.01f;
						float HitLength;
						if (IntersectRayWithBox(Start, Direction, EmptyLeaves[j]->NodeBounds, HitLength, MaxLength))
						{
							if (HitLength < MaxLength)
							{
								Neighbours.Add(EmptyLeaves[j]);
							}
						}
					}
				}
			}
		}
		for (const auto& Neighbour : Neighbours)
		{
			NavigationGraph->AddEdge(EmptyLeaves[i], Neighbour);
		}

    	PrestoLOG::Log("Neighbours: ", Neighbours.Num());
    }

}

bool AOctreeActor::IntersectRayWithBox(const FVector& RayOrigin, const FVector& RayDirection, const FBox& Box, float& HitLength, float MaxLength)
{
	FVector BoxMin = Box.Min;
	FVector BoxMax = Box.Max;
	
	float t1 = (BoxMin.X - RayOrigin.X) / RayDirection.X;
	float t2 = (BoxMax.X - RayOrigin.X) / RayDirection.X;
	float t3 = (BoxMin.Y - RayOrigin.Y) / RayDirection.Y;
	float t4 = (BoxMax.Y - RayOrigin.Y) / RayDirection.Y;
	float t5 = (BoxMin.Z - RayOrigin.Z) / RayDirection.Z;
	float t6 = (BoxMax.Z - RayOrigin.Z) / RayDirection.Z;

	float tmin = FMath::Max(FMath::Max(FMath::Min(t1, t2), FMath::Min(t3, t4)), FMath::Min(t5, t6));
	float tmax = FMath::Min(FMath::Min(FMath::Max(t1, t2), FMath::Max(t3, t4)), FMath::Max(t5, t6));
	
	if (tmax >= 0 && tmin <= tmax)
	{
		HitLength = tmin;
		if (HitLength <= MaxLength)
		{
			return true;
		}
	}
	return false;
}

void AOctreeActor::ConnectLeafNodeNeighboursMultithreaded()
{
	int MaxThreads = FPlatformMisc::NumberOfCores();
	if (EmptyLeaves.Num() <= MaxThreads)
	{
		ConnectLeafNodeNeighbours();
		return;
	}

	//int MaxThreads = 4;

	const int Start = 0;
	const int End = EmptyLeaves.Num() - 1;
	// Create runnables and threads
	for (int i = 1; i <= MaxThreads; ++i)
	{
		int SubRangeSize = (End - Start + 1) / MaxThreads;
		int SubRangeStart = Start + (i - 1) * SubRangeSize;
		int SubRangeEnd = SubRangeStart + SubRangeSize - 1;
		
		// The last thread may get a slightly larger subrange to cover any remainder
		if (i == MaxThreads)
		{
			SubRangeEnd = End;
		}
		
		TUniquePtr<EdgesRunnable> ERunnable = MakeUnique<EdgesRunnable>(i, SubRangeStart, SubRangeEnd, this);
		FString ThreadName = FString::Printf(TEXT("Thread_%d"), i);
		FRunnableThread* Threado = FRunnableThread::Create(ERunnable.Get(), *ThreadName, 0, TPri_Highest);
		if (Threado)
		{
			EdgesRunnables.Add(MoveTemp(ERunnable));
			Threads.Add(Threado);
		}
	}
	// Wait for threads to finish
	for (auto& Thread : Threads)
	{
		if (Thread)
		{
			Thread->WaitForCompletion();
			delete Thread;
		}
	}
}

void AOctreeActor::ProcessLeafNodes(int startIndex, int endIndex)
{
	TArray<FVector> Directions;
	Directions.Add(FVector(1.f, 0.f, 0.f));
	Directions.Add(FVector(-1.f, 0.f, 0.f));
	Directions.Add(FVector(0.f, 1.f, 0.f));
	Directions.Add(FVector(0.f, -1.f, 0.f));
	Directions.Add(FVector(1.f, 0.f, 1.f));
	Directions.Add(FVector(1.f, 0.f, -1.f));

	for (int32 i = startIndex; i < endIndex; ++i)
	{
		TArray<FNodeOctree*> Neighbours;
		for (int32 j = startIndex = 0; j < endIndex; ++j)
		{
			if (i != j)
			{
				FVector Start = EmptyLeaves[i]->Center;
				// Calculate the distance between the current leaf node and the potential neighbor
				const float Distance = FVector::Distance(Start, EmptyLeaves[j]->Center);
            
				// Calculate a dynamic maximum distance threshold based on the size of the leaf node's bounding box
				const float MaxSizeThreshold = EmptyLeaves[i]->NodeBounds.GetExtent().Size() + EmptyLeaves[j]->NodeBounds.GetExtent().Size();
				if (Distance < MaxSizeThreshold)
				{
					for (int32 r = 0; r < 6; ++r)
					{
						FVector Direction = Directions[r];
						const float MaxLength = EmptyLeaves[i]->NodeBounds.GetExtent().Size() + 0.01f;
						float HitLength;
						if (IntersectRayWithBox(Start, Direction, EmptyLeaves[j]->NodeBounds, HitLength, MaxLength))
						{
							if (HitLength < MaxLength)
							{
								// Intersection found
								Neighbours.Add(EmptyLeaves[j]);
							}
						}
					}
				}
			}
		}
		// Add Edges
		for (const auto& Neighbour : Neighbours)
		{
			NavigationGraph->AddEdge(EmptyLeaves[i], Neighbour);
		}

		PrestoLOG::Log("Neighbours: ", Neighbours.Num());
	}

}

// ----------------------------------------------------------------------------------- //


void AOctreeActor::RemoveActorFromOctreeBP(AActor* ActorToRemove)
{
	RemoveActorFromOctree(ActorToRemove, RootNode);
}

 void AOctreeActor::RemoveActorFromOctree(AActor* ActorToRemove, FNodeOctree*& CurrentNode)
 {
 	if (!CurrentNode)
 		return;
 
 	// If it's a leaf node and contains the actor, remove the actor
 	if (CurrentNode->IsLeafNode() && CurrentNode->ContainsActor(ActorToRemove))
 	{
 		CurrentNode->RemoveActor(ActorToRemove);
 		if (!CurrentNode->HasActors())
 		{
 			CurrentNode->bIsOccupied = false;
 		}
 	}
	
 	if (!CurrentNode->IsLeafNode())
 	{
 		for (int32 i = 0; i < 8; ++i)
 		{
 			if (CurrentNode->Children[i] != nullptr)
 			{
 				RemoveActorFromOctree(ActorToRemove, CurrentNode->Children[i]);
 			}
 		}
 	}
 }

void AOctreeActor::PurgeEmptyBranches(TArray<FNodeOctree*>& InOctreeNodes)
{
	// Nodes to Delete
	for (int32 i = InOctreeNodes.Num() - 1; i >= 0; --i)
	{
		FNodeOctree* NodeToDelete = InOctreeNodes[i];
		PurgeBranchIfEmpty(NodeToDelete);
	}
	
	InOctreeNodes.Empty();
}

void AOctreeActor::PurgeBranchIfEmpty(FNodeOctree*& Node)
{
	if (Node == nullptr || Node->Parent == nullptr || RootNode->Children.Contains(Node))
		return;
	
	PurgeBranchIfEmpty(Node->Parent);

	for (int32 i = 0; i < Node->Parent->Children.Num(); ++i)
	{
		if (Node->Parent->Children[i] == Node)
		{
			delete Node->Parent->Children[i];
			Node->Parent->Children.RemoveAt(i);
			break;
		}
	}
}

void AOctreeActor::DeleteAllNodesFromOctree()
{
	if (RootNode != nullptr)
	{
		RootNode->RemoveAllOctreeNodes();
	}
}

void AOctreeActor::MarkNodesAsUnOccupiedForActor(const AActor* InActor) const
{
	if (RootNode != nullptr)
	{
		RootNode->MarkNodesAsNotOccupied(InActor);
	}
}


void AOctreeActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Threads.Empty();
	EdgesRunnables.Empty();
	delete RootNode;
}

