// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StateTree/FlyingStateTreeComponent.h"

UFlyingStateTreeComponent::UFlyingStateTreeComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UFlyingStateTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UFlyingStateTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFlyingStateTreeComponent::SetStateTree(UStateTree* InStateTree)
{
	StateTreeRef.SetStateTree(InStateTree);
	InitializeComponent();
}

