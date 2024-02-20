// Fill out your copyright notice in the Description page of Project Settings.


#include "Runnables/EdgesRunnable.h"

#include "Octree//OctreeActor.h"
#include "Octree/GraphEntity.h"


EdgesRunnable::~EdgesRunnable()
{
	//delete AddEdgeCriticalSection;
}

bool EdgesRunnable::Init()
{
	bStopThread = false;
	return FRunnable::Init();
}

uint32 EdgesRunnable::Run()
{
	auto StartTime = FDateTime::UtcNow();
	TArray<FVector> Directions;
	Directions.Add(FVector(1.f, 0.f, 0.f));
	Directions.Add(FVector(-1.f, 0.f, 0.f));
	Directions.Add(FVector(0.f, 1.f, 0.f));
	Directions.Add(FVector(0.f, -1.f, 0.f));
	Directions.Add(FVector(1.f, 0.f, 1.f));
	Directions.Add(FVector(1.f, 0.f, -1.f));
	UE_LOG(LogTemp, Warning, TEXT("Thread %d StartIndex: "), StartIndex);
	UE_LOG(LogTemp, Warning, TEXT("Thread %d EndIndex: "), EndIndex);
	
	for (int32 i = StartIndex; i <= EndIndex; ++i)
	{
		TArray<FNodeOctree*> Neighbours;
		for (int32 j = 0; j < OctreeActor->EmptyLeaves.Num(); ++j)
		{
			if (i != j)
			{
				FVector Start = OctreeActor->EmptyLeaves[i]->Center;
				// Calculate the distance between the current leaf node and the potential neighbor
				const float Distance = FVector::Distance(Start, OctreeActor->EmptyLeaves[j]->Center);
           
				// Calculate a dynamic maximum distance threshold based on the size of the leaf node's bounding box
				const float MaxSizeThreshold = OctreeActor->EmptyLeaves[i]->NodeBounds.GetExtent().Size() + OctreeActor->EmptyLeaves[j]->NodeBounds.GetExtent().Size();
				if (Distance < MaxSizeThreshold)
				{
					for (int32 r = 0; r < 6; ++r)
					{
						// Create a ray
						FVector Direction = Directions[r];
						const float MaxLength = OctreeActor->EmptyLeaves[i]->NodeBounds.GetExtent().Size() + 0.01f;
						float HitLength;
						if (IntersectRayWithBox(Start, Direction, OctreeActor->EmptyLeaves[j]->NodeBounds, HitLength, MaxLength))
						{
							if (HitLength < MaxLength)
							{
								// Intersection found, add to neighbors
								Neighbours.Add(OctreeActor->EmptyLeaves[j]);
							}
						}
					}
				}
			}
		}
		
		for (const auto& Neighbour : Neighbours)
		{			
			OctreeActor->NavigationGraph->AddEdge(OctreeActor->EmptyLeaves[i], Neighbour);
			//++NumberCounter;
		}
	}

	const auto EndTime = FDateTime::UtcNow();
	const FTimespan Duration = EndTime - StartTime;

	UE_LOG(LogTemp, Warning, TEXT("Thread %d took %f milliseconds."), ThreadId, Duration.GetTotalMilliseconds());

	return 0;
}

void EdgesRunnable::Stop()
{
	FRunnable::Stop();
}

void EdgesRunnable::Exit()
{
	
}

bool EdgesRunnable::IntersectRayWithBox(const FVector& RayOrigin, const FVector& RayDirection, const FBox& Box, float& HitLength, float MaxLength)
{
	const FVector BoxMin = Box.Min;
	const FVector BoxMax = Box.Max;

	float t1 = (BoxMin.X - RayOrigin.X) / RayDirection.X;
	float t2 = (BoxMax.X - RayOrigin.X) / RayDirection.X;
	float t3 = (BoxMin.Y - RayOrigin.Y) / RayDirection.Y;
	float t4 = (BoxMax.Y - RayOrigin.Y) / RayDirection.Y;
	float t5 = (BoxMin.Z - RayOrigin.Z) / RayDirection.Z;
	float t6 = (BoxMax.Z - RayOrigin.Z) / RayDirection.Z;

	float Tmin = FMath::Max(FMath::Max(FMath::Min(t1, t2), FMath::Min(t3, t4)), FMath::Min(t5, t6));
	float Tmax = FMath::Min(FMath::Min(FMath::Max(t1, t2), FMath::Max(t3, t4)), FMath::Max(t5, t6));

	// If the ray intersects the box
	if (Tmax >= 0 && Tmin <= Tmax)
	{
		// Calculate the hit length
		HitLength = Tmin;
		// If the hit length is within the maximum length
		if (HitLength <= MaxLength)
		{
			return true;
		}
	}
	return false;
	
}