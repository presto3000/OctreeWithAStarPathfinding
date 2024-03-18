// presto3000 Chris Pawlowski

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AI/TestFlyingPawn.h"
#include "RestingTask.generated.h"

enum class EFlyingPawnState : uint8;
class ATestFlyingPawn;

USTRUCT()
struct OCTREEPATHFINDING_API FRestingTaskInstanceData
{
	GENERATED_BODY()

	/** Context Actor. */
	UPROPERTY(EditAnywhere, Category = "Input", meta=(Optional))
	TObjectPtr<ATestFlyingPawn> ReferenceActor = nullptr;

	/** RotationThreshold */
	UPROPERTY(EditAnywhere, Category = Parameter)
	float RotationThreshold = 720.f;

	/** RotationToAddPerSec */
	UPROPERTY(EditAnywhere, Category = Parameter)
	float RotationToAddPerSec = 720.f;

	/** RotationAdded Value. */
	float RotationAdded = 0.f;

	UPROPERTY(EditAnywhere, Category = Parameter, meta=(Optional))
	EFlyingPawnState FlyingPawnStateOnSuccess = EFlyingPawnState::Waiting;

};

/**
 * 	 Simple Resting job task to perform some kind of Barrel roll 720 :)
 *
*/

USTRUCT(meta = (DisplayName = "Resting Task"))
struct OCTREEPATHFINDING_API FRestingTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FRestingTaskInstanceData;

	FRestingTask() = default;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;


};