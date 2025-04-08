// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FMG_PlayerState.h"
#include "Components/TextBlock.h"
#include "Character/BaseCharacter.h"
#include "PlayerStatus.generated.h"

UCLASS()
class FIGHTERMULTIGAME_API UPlayerStatus : public UUserWidget
{
	GENERATED_BODY()

	
public:

	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerImage;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HPValueText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* LifeValueText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CharacterNameText;

	UFUNCTION()
	void UpdateHP(float NewHP);

	void SetPlayerInfo(class ABaseCharacter* InCharacter);
	
	ABaseCharacter* GetCharacter(){return Character;}

private:
	
	UPROPERTY()
	ABaseCharacter* Character;

	
	UPROPERTY()
	AFMG_PlayerState* BoundPlayerState ;
};
