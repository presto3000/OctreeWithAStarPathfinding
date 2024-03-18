// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TestFlyingPawn.h"

#include "Actors/GoalActor.h"
#include "Octree/OctreeActor.h"
#include "Kismet/GameplayStatics.h"
#include "Octree/GraphEntity.h"
#include "Octree/Node.h"


ATestFlyingPawn::ATestFlyingPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
}

void ATestFlyingPawn::BeginPlay()
{
	Super::BeginPlay();
	
}


void ATestFlyingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UpdateMovement(DeltaTime);
	UpdateMovement2(DeltaTime);
}

void ATestFlyingPawn::UpdateMovement2(float DeltaTime)
{
	if (bIsMoving)
	{
		if (Graph == nullptr)
		{
			return;
		}
		if (GetPathLength() == 0 || CurrentWP == GetPathLength())
		{
			bHasReachedDestination = true;
			return;
		}
		// We are close enough
		if (FVector::Distance(GetPathPoint(CurrentWP)->Center, GetActorLocation()) <= Accuracy)
		{
			CurrentWP++;
		}
	
		if (CurrentWP < GetPathLength())
		{
			bHasReachedDestination = false;
			Goal = GetPathPoint(CurrentWP)->Center;
			CurrentNode = GetPathPoint(CurrentWP);
		
			const FVector LookAtGoal{Goal.X, Goal.Y, Goal.Z};
			FVector Direction = LookAtGoal - GetActorLocation();
			Direction.Normalize();
		
			// Rotation using Slerp
			const FQuat CurrentRotation = GetActorRotation().Quaternion();
			const FQuat TargetRotation = FQuat::FindBetweenNormals(FVector::ForwardVector, Direction); // Find the rotation to look in the direction
			const FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, DeltaTime * RotSpeed);
			SetActorRotation(NewRotation.Rotator());
		
			// Move the Pawn
			const FVector MovementDirection = GetActorForwardVector();
			const FVector NewLocation = GetActorLocation() + MovementDirection * Speed * DeltaTime;
			SetActorLocation(NewLocation);
		}
		else
		{
			if (GetPathLength() == 0)
			{
				bHasReachedDestination = false;
				PrestoLOG::Log("NO PATH");
			}
		}
	}
}

int32 ATestFlyingPawn::GetPathLength() const
{
	return PathList.Num();
}

FNodeOctree* ATestFlyingPawn::GetPathPoint(const int32 InIndex)
{
	return PathList[InIndex]->OctreeNode;
}


void ATestFlyingPawn::Navigate()
{
	if (UWorld* World = GetWorld())
	{
		OctreeActor = Cast<AOctreeActor>(UGameplayStatics::GetActorOfClass(World, AOctreeActor::StaticClass()));
		if (OctreeActor && OctreeActor->NavigationGraph)
		{
			Graph = Graph == nullptr ? OctreeActor->NavigationGraph : Graph;
			//CurrentNode = Graph->Nodes[CurrentWP]->OctreeNode;
			//GetRandomDestination();
		}
		
		GoalActor = Cast<AGoalActor>(UGameplayStatics::GetActorOfClass(World, AGoalActor::StaticClass()));
		if (GoalActor)
		{
			GoalPosition = GoalActor->GetActorLocation();
		}
		Update(GoalActor);
		
	}

}

void ATestFlyingPawn::Update(AGoalActor* InGoalActor)
{
	if (OctreeActor && GoalPosition != FVector::ZeroVector && InGoalActor)
	{
		const int32 StartingId = OctreeActor->AddDestination(GetActorLocation());
		const int32 DestinationId = OctreeActor->AddDestination(GoalPosition);
		
		if (DestinationId == -1)
		{
			PrestoLOG::Log(("Destination not found in Octree."));
			return;
		}
		
		if (StartingId == -1)
		{
			PrestoLOG::Log(("StartingId not found in Octree."));
			return;
		}
		
		const FBox Bounds = InGoalActor->GetComponentsBoundingBox();
		FNodeAStar* FinalGoal = new FNodeAStar{new FNodeOctree{Bounds, 1, nullptr}};
		
		NavigateTo(StartingId, DestinationId, FinalGoal);
		//DebugPath(-1, true);
	}
}

void ATestFlyingPawn::NavigateTo(const int32 StartId, const int32 DestinationId, FNodeAStar* FinalGoal)
{
	const FNodeAStar* StartNode = Graph->FindNode(StartId);
	const FNodeAStar* DestinationNode = Graph->FindNode(DestinationId);
	
	if (StartNode && DestinationNode)
	{
		Graph->AStar(StartNode->OctreeNode, DestinationNode->OctreeNode, PathList);
		CurrentWP = 0;
		PathList.Add(FinalGoal);
	}

}

void ATestFlyingPawn::GetRandomDestination()
{
	if (Graph == nullptr)
	{
		return;
	}
	
	while (true)
	{
		const int32 RandNode = FMath::RandRange(0, Graph->Nodes.Num() - 1);
		if (!Graph->Nodes[RandNode]->OctreeNode->bIsOccupied)
		{
			const double Start = FPlatformTime::Seconds();
			Graph->AStar(Graph->Nodes[CurrentWP]->OctreeNode, Graph->Nodes[RandNode]->OctreeNode, PathList);
			const double End = FPlatformTime::Seconds();

			UE_LOG(LogTemp, Warning, TEXT("A* Path created in: %f ms"), (End - Start) * 1000.f);

			CurrentWP = 0;
			// Unoccupied node is found. Break here
			break; 
		}
	}
	
}

void ATestFlyingPawn::FindRandomDestinationAndPath()
{
	if (Graph == nullptr)
	{
		return;
	}

	// If the node is occupied, the loop continues to find another random node.
	while (true)
	{
		const int32 RandNode = FMath::RandRange(0, Graph->Nodes.Num() - 1);
		if (!Graph->Nodes[RandNode]->OctreeNode->bIsOccupied)
		{
			const double Start = FPlatformTime::Seconds();
			const int32 StartingId = OctreeActor->AddDestination(GetActorLocation());
			const FNodeAStar* StartNode = Graph->FindNode(StartingId);
			if (StartNode && Graph->Nodes[RandNode]->OctreeNode)
			{
				Graph->AStar(StartNode->OctreeNode, Graph->Nodes[RandNode]->OctreeNode, PathList);
				const double End = FPlatformTime::Seconds();
				//DebugPath(5, true);
				UE_LOG(LogTemp, Warning, TEXT("A* Path created in: %f ms"), (End - Start) * 1000.f);
				CurrentWP = 0;
			}
			// Unoccupied node is found. Break here
			break; 
		}
	}
}

void ATestFlyingPawn::UpdateMovement(float DeltaTime)
{
	if (Graph == nullptr)
	{
		return;
	}

	if (GetPathLength() == 0 || CurrentWP == GetPathLength())
	{
		GetRandomDestination();
		return;
	}
		
	// We are close enough
	if (FVector::Distance(GetPathPoint(CurrentWP)->Center, GetActorLocation()) <= Accuracy)
	{
		CurrentWP++;
	}


	if (CurrentWP < GetPathLength())
	{
		Goal = GetPathPoint(CurrentWP)->Center;
		CurrentNode = GetPathPoint(CurrentWP);
		
		const FVector LookAtGoal{Goal.X, Goal.Y, Goal.Z};
		FVector Direction = LookAtGoal - GetActorLocation();
		Direction.Normalize();
		
		// Rotation using Slerp
		const FQuat CurrentRotation = GetActorRotation().Quaternion();
		const FQuat TargetRotation = FQuat::FindBetweenNormals(FVector::ForwardVector, Direction); // Find the rotation to look in the direction
		const FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, DeltaTime * RotSpeed);

		SetActorRotation(NewRotation.Rotator());
		// Move the Pawn
		const FVector MovementDirection = GetActorForwardVector();
		const FVector NewLocation = GetActorLocation() + MovementDirection * Speed * DeltaTime;
		SetActorLocation(NewLocation);
	}
	else
	{
		GetRandomDestination();
		if (GetPathLength() == 0)
		{
			PrestoLOG::Log("NO PATH");
		}
	}
	
}


void ATestFlyingPawn::DebugPath(float InDuration, bool InDrawPoints)
{
	const bool Persistent = InDuration < 0;
	for (int i = 0; i < PathList.Num() - 1; ++i)
	{
		DrawDebugLine(GetWorld(), PathList[i]->OctreeNode->Center, PathList[i + 1]->OctreeNode->Center, FColor::Magenta, Persistent, InDuration, 0, 4);
		if (InDrawPoints)
		{
			DrawDebugPoint(GetWorld(), PathList[i]->OctreeNode->Center, 20, FColor::Emerald, Persistent, InDuration);
		}
	}
}




