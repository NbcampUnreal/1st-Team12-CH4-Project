// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KchTestGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Character/TestPlayerCharacter.h"

AKchTestGameMode::AKchTestGameMode()
{
	DefaultPawnClass = ATestPlayerCharacter::StaticClass();
}

AActor* AKchTestGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (!PlayerIndexMap.Contains(Player))
	{
		PlayerIndexMap.Add(Player, NextPlayerIndex++);
	}

	const int32 PlayerIndex = PlayerIndexMap[Player];
	const FString ExpectedStartTag = FString::Printf(TEXT("PlayerStart_%d"), PlayerIndex);

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* Start = *It;

		if (Start->GetName().Contains(ExpectedStartTag))
		{
			UE_LOG(LogTemp, Log, TEXT("%d번 플레이어 %s 위치시작"), PlayerIndex, *Start->GetName());
			return Start;
		}
	}

	// 기본 PlayerStart 사용
	return Super::ChoosePlayerStart_Implementation(Player);
}
