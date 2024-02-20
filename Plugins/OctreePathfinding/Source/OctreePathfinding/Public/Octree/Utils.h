// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FUtils
{
public:
	// Declare a public static integer
	inline static int Id_Number = 0;
};

// Using LexToString
namespace PrestoLOG
{
	inline FString CombinedLog;
	
	template <typename T>
	void Log(T t) {
		CombinedLog += LexToString(t);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *CombinedLog);
		CombinedLog = "";
	}

	template <typename T, typename... Args>
	void Log(T t, Args... args)
	{
		CombinedLog += LexToString(t);
		Log(args...);
	}
	
	inline FString CombinedScreenLog;
	template <typename T>
	void ScreenLog5(T t) {
		CombinedScreenLog += LexToString(t);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, *CombinedScreenLog);
		}
		CombinedScreenLog = "";
	}

	template <typename T, typename... Args>
	void ScreenLog5(T t, Args... args)
	{
		CombinedScreenLog += LexToString(t);
		ScreenLog5(args...);
	}
}
