// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerStatus.h"

#include "Components/Image.h"


void UPlayerStatus::UpdateHP(float NewHP)
{
	if (HPValueText)
	{
		UE_LOG(LogTemp, Warning, TEXT("HP 갱신됨: %.1f"), NewHP);
		HPValueText->SetText(FText::AsNumber(FMath::RoundToInt(NewHP)));
	}
}

void UPlayerStatus::SetPlayerInfo(class  ABaseCharacter* InCharacter)
{
	
	UE_LOG(LogTemp, Warning, TEXT("UPlayerStatu-SetPlayerInfo 호출 %s"), *InCharacter->CharacterName);

	Character = InCharacter;
	
	BoundPlayerState = Cast<AFMG_PlayerState>(InCharacter->GetPlayerState());

	if (CharacterNameText)
	{
		CharacterNameText->SetText(FText::FromString(InCharacter->CharacterName));
	}

	if (HPValueText)
	{
		HPValueText->SetText(FText::AsNumber(FMath::RoundToInt(InCharacter->GetHP())));
	}

	if (LifeValueText)
	{
		LifeValueText->SetText(FText::AsNumber(5));
	}

	if (PlayerImage && InCharacter->CharacterIcon)
	{
		PlayerImage->SetBrushFromTexture(InCharacter->CharacterIcon);
	}

	//Ai,플레이어에따라 나눈  UI
	if (BoundPlayerState)
	{
		if (!BoundPlayerState->OnHPChanged.IsAlreadyBound(this, &UPlayerStatus::UpdateHP))
		{
			BoundPlayerState->OnHPChanged.RemoveDynamic(this, &UPlayerStatus::UpdateHP);
			BoundPlayerState->OnHPChanged.AddDynamic(this, &UPlayerStatus::UpdateHP);
		}
	}
	else if (Character)
	{
		if (!Character->OnHPChanged.IsAlreadyBound(this, &UPlayerStatus::UpdateHP))
		{
			Character->OnHPChanged.RemoveDynamic(this, &UPlayerStatus::UpdateHP);
			Character->OnHPChanged.AddDynamic(this, &UPlayerStatus::UpdateHP);
		}
	}
	
	UpdateHP(Character->GetHP());
	UpdateHP(BoundPlayerState->GetHP());
}
