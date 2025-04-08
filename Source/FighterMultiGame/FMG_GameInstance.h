// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FMG_GameInstance.generated.h"


UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	SinglePlayer   UMETA(DisplayName = "Single Player"),
	Multiplayer    UMETA(DisplayName = "Multiplayer")
};

UCLASS()
class FIGHTERMULTIGAME_API UFMG_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    UFMG_GameInstance();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Mode")
    EGameModeType SelectedGameMode;

    // 싱글플레이 플레이어 인덱스
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Select")
    int32 PlayerCharacterIndex = -1;

    // 싱글플레이 AI 인덱스
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Select")
    int32 AICharacterIndex = -1;
};
