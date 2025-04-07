// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KchTestGameMode.h"

#include "EngineUtils.h"
#include "FMG_PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Character/TestPlayerCharacter.h"

class AFMG_PlayerState;

AKchTestGameMode::AKchTestGameMode()
{
	DefaultPawnClass = ATestPlayerCharacter::StaticClass();
}

void AKchTestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (APlayerState* PS = NewPlayer->PlayerState)
	{
		if (AFMG_PlayerState* FMG_PS = Cast<AFMG_PlayerState>(PS))
		{
			FMG_PS->PlayerIndex = NextPlayerIndex++;
			UE_LOG(LogTemp, Log, TEXT("플레이어 인덱스 할당: %d"), FMG_PS->PlayerIndex);
		}
	}
}

AActor* AKchTestGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AFMG_PlayerState* PS = Cast<AFMG_PlayerState>(Player->PlayerState);
	if (!PS) return Super::ChoosePlayerStart_Implementation(Player);

	int32 PlayerIndex = PS->PlayerIndex; 

	const FString ExpectedStartTag = FString::Printf(TEXT("PlayerStart_%d"), PlayerIndex);

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->GetName().Contains(ExpectedStartTag))
		{
			UE_LOG(LogTemp, Log, TEXT("%d번 플레이어 %s 위치시작"), PlayerIndex, *It->GetName());
			return *It;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}
