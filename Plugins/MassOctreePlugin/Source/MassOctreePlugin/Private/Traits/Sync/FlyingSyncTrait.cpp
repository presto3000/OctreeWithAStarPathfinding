// Fill out your copyright notice in the Description page of Project Settings.


#include "Traits/Sync/FlyingSyncTrait.h"

#include "MassEntityTemplateRegistry.h"
#include "MassEntityView.h"
#include "Translators/MassSceneComponentLocationTranslator.h"

void UFlyingSyncTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FMassSceneComponentWrapperFragment>();
	BuildContext.AddFragment<FTransformFragment>();


	BuildContext.GetMutableObjectFragmentInitializers().Add([=](UObject& Owner, FMassEntityView& EntityView, const EMassTranslationDirection CurrentDirection)
		{
			AActor* AsActor = Cast<AActor>(&Owner);
			if (AsActor && AsActor->GetRootComponent())
			{
				USceneComponent* Component = AsActor->GetRootComponent();
				FMassSceneComponentWrapperFragment& ComponentFragment = EntityView.GetFragmentData<FMassSceneComponentWrapperFragment>();
				ComponentFragment.Component = Component;

				FTransformFragment& TransformFragment = EntityView.GetFragmentData<FTransformFragment>();

				REDIRECT_OBJECT_TO_VLOG(Component, &Owner);
				UE_VLOG_LOCATION(&Owner, LogMass, Log, Component->GetComponentLocation(), 30, FColor::Yellow, TEXT("Initial component location"));
				UE_VLOG_LOCATION(&Owner, LogMass, Log, TransformFragment.GetTransform().GetLocation(), 30, FColor::Red, TEXT("Initial entity location"));

				// the entity is the authority
				if (CurrentDirection == EMassTranslationDirection::MassToActor)
				{
					// Temporary disabling this as it is already done earlier in the MassRepresentation and we needed to do a sweep to find the floor
					//Component->SetWorldLocation(FeetLocation, /*bSweep*/true, nullptr, ETeleportType::TeleportPhysics);
				}
				// actor is the authority
				else
				{
					TransformFragment.GetMutableTransform().SetLocation(Component->GetComponentTransform().GetLocation() - FVector(0.f, 0.f, Component->Bounds.BoxExtent.Z));
				}
			}
		});

	if (EnumHasAnyFlags(SyncDirection, EMassTranslationDirection::ActorToMass))
	{
		BuildContext.AddTranslator<UMassSceneComponentLocationToMassTranslator>();
	}

	if (EnumHasAnyFlags(SyncDirection, EMassTranslationDirection::MassToActor))
	{
		BuildContext.AddTranslator<UMassSceneComponentLocationToActorTranslator>();
	}
}
