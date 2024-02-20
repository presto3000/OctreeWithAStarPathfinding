// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

struct FNodeOctree;
class AOctreeActor;
/**
 * 
 */
class  EdgesRunnable : public FRunnable
{
public:
	EdgesRunnable(int InThreadId, int InStart, int InEnd, AOctreeActor* InOctreeActor)
		: bStopThread(false), ThreadId(InThreadId), StartIndex(InStart), EndIndex(InEnd),
		  OctreeActor(InOctreeActor)
	{
		// Initialize the AddEdgeCriticalSection in the constructor
		//AddEdgeCriticalSection = new FCriticalSection();
	}
	~EdgesRunnable();
 bool bStopThread;
	
  	virtual bool Init() override;
  	virtual uint32 Run() override;
  	virtual void Stop() override;
	
	virtual void Exit() override;
	
	bool IntersectRayWithBox(const FVector& RayOrigin, const FVector& RayDirection, const FBox& Box, float& HitLength,
	                         float MaxLength);

private:
	
	int ThreadId;
	int StartIndex;
	int EndIndex;

	AOctreeActor* OctreeActor;
	//std::atomic<int32> NumberCounter = 0;
	
};
