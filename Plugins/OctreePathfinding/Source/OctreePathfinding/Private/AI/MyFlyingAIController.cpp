// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyFlyingAIController.h"

#include "AI/TestFlyingPawn.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"


// Sets default values
AMyFlyingAIController::AMyFlyingAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyFlyingAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyFlyingAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ATestFlyingPawn* FlyingPawn = Cast<ATestFlyingPawn> (InPawn))
	{
		if (UBehaviorTree* Tree = FlyingPawn->GetBehaviourTree())
		{
			UBlackboardComponent* BlackboardComponent;
			UseBlackboard(Tree->BlackboardAsset, BlackboardComponent);
			Blackboard = BlackboardComponent;
			RunBehaviorTree(Tree);
		}
	}
}

// Called every frame
void AMyFlyingAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

