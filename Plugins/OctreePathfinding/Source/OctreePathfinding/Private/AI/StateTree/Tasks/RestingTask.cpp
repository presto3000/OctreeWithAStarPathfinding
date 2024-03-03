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
		InstanceData.RotationAdded += InstanceData.RotationToAddEachTick;
		InstanceData.ReferenceActor->GetMesh()->AddLocalRotation(FRotator{InstanceData.RotationToAddEachTick, 0, 0});
		return EStateTreeRunStatus::Running;
	}
}