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

void AKchTestGameMode::CheckTeamAllDead()
{
	int32 AliveA = 0;
	int32 AliveB = 0;

	for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
	{
		ABaseCharacter* Fighter = *It;
		if (!Fighter || Fighter->bIsDead == true) continue;

		if (AFMG_PlayerState* PS = Cast<AFMG_PlayerState>(Fighter->GetPlayerState()))
		{
			if (IsTeamA(PS->PlayerIndex))
			{
				AliveA++;
			}
			else if (IsTeamB(PS->PlayerIndex))
			{
				AliveB++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("A팀 생존: %d / B팀 생존: %d"), AliveA, AliveB);

	if (AliveA == 0)
	{
		OnTeamDefeated(TEXT("A팀"));
	}
	else if (AliveB == 0)
	{
		OnTeamDefeated(TEXT("B팀"));
	}
}
