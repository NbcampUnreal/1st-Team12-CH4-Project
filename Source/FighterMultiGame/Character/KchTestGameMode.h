// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "KchTestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERMULTIGAME_API AKchTestGameMode : public AGameMode
{
	GENERATED_BODY()

	AKchTestGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	TMap<AController*, int32> PlayerIndexMap;
	int32 NextPlayerIndex = 0;
};
