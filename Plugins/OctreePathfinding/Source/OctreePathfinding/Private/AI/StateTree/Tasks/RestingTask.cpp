// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StateTree/Tasks/RestingTask.h"
#include "StateTreeExecutionContext.h"
#include "AI/TestFlyingPawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RestingTask)

EStateTreeRunStatus FRestingTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ReferenceActor->SetFlyingPawnState(EFlyingPawnState::Resting);
	return EStateTreeRunStatus::Running;

}

EStateTreeRunStatus FRestingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (InstanceData.RotationAdded >= InstanceData.RotationThreshold)
	{
		InstanceData.ReferenceActor->SetFlyingPawnState(InstanceData.FlyingPawnStateOnSuccess);
		InstanceData.RotationAdded = 0;
		return EStateTreeRunStatus::Succeeded;
	}
	else
	{
		const float RemainingRotation = InstanceData.RotationThreshold - InstanceData.RotationAdded;
		const float RotationToAddThisFrame = FMath::Min(InstanceData.RotationToAddPerSec * DeltaTime, RemainingRotation);
		InstanceData.RotationAdded += RotationToAddThisFrame;
		InstanceData.ReferenceActor->GetMesh()->AddLocalRotation(FRotator{RotationToAddThisFrame, 0, 0});

		if (InstanceData.RotationAdded >= InstanceData.RotationThreshold)
		{
			InstanceData.ReferenceActor->SetFlyingPawnState(InstanceData.FlyingPawnStateOnSuccess);
			InstanceData.RotationAdded = 0;
			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			return EStateTreeRunStatus::Running;
		}
	}
}