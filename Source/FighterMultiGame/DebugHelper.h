// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FIGHTERMULTIGAME_API DebugHelper
{
public:
	DebugHelper();
	~DebugHelper();

	static FString GetNetModeName(UWorld* World);
};
