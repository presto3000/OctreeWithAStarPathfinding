// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawners/OctreeMassSpawner.h"

AOctreeMassSpawner::AOctreeMassSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AOctreeMassSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOctreeMassSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

