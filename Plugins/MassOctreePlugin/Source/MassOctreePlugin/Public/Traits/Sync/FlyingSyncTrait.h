// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassAgentTraits.h"
#include "FlyingSyncTrait.generated.h"

/**
 * 
 */
UCLASS()
class MASSOCTREEPLUGIN_API UFlyingSyncTrait : public UMassAgentSyncTrait
{
	GENERATED_BODY()

	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
