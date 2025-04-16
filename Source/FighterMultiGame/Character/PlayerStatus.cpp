// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerStatus.h"

#include "DebugHelper.h"
#include "Components/Image.h"



void UPlayerStatus::UpdateHP(float NewHP)
{
	
	if (HPValueText)
	{
		HPValueText->SetText(FText::AsNumber(FMath::RoundToInt(NewHP)));
	}
}

void UPlayerStatus::SetPlayerInfo(class  ABaseCharacter* InCharacter)
{
	

	if (!IsValid(InCharacter))
	{
		SetVisibility(ESlateVisibility::Collapsed);
		Character = nullptr;
		return;
	}
	
	SetVisibility(ESlateVisibility::Visible);
	Character = InCharacter;
	if (CharacterNameText)
		CharacterNameText->SetText(FText::FromString(Character->CharacterName));
	if (HPValueText)
		HPValueText->SetText(FText::AsNumber(FMath::RoundToInt(Character->GetHP())));
	if (PlayerImage && Character->CharacterIcon)
		PlayerImage->SetBrushFromTexture(Character->CharacterIcon);
	if (LifeValueText)
		LifeValueText->SetText(FText::AsNumber(5));

	UpdateHP(Character->GetHP());

}
