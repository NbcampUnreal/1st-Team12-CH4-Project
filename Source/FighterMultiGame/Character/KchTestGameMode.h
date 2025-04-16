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


	

public:
	
	// 모든 캐릭터의 생존 상태 체크 함수
	UFUNCTION(BlueprintCallable)
	void CheckTeamAllDead();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Game")
	void OnTeamDefeated(const FString& DefeatedTeam);
	
private:
	TMap<AController*, int32> PlayerIndexMap;
	int32 NextPlayerIndex = 0;

	bool IsTeamA(int32 Index) { return Index == 0 || Index == 1; }
	bool IsTeamB(int32 Index) { return Index == 2 || Index == 3; }
};
