// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerStatus.h"


void UPlayerStatus::UpdateHP(float NewHP)
{
	if (HPValueText)
	{
		UE_LOG(LogTemp, Warning, TEXT("HP 갱신됨: %.1f"), NewHP);
    
		HPValueText->SetText(FText::AsNumber(FMath::RoundToInt(NewHP)));
	}
}

void UPlayerStatus::SetPlayerInfo(class AFMG_PlayerState* InPlayerState)
{
	BoundPlayerState = InPlayerState;
	UE_LOG(LogTemp, Warning, TEXT("[UPlayerStatus] SetPlayerInfo 호출됨: %s"), *InPlayerState->PlayerNickname);

	if (CharacterNameText)
	{
		CharacterNameText->SetText(FText::FromString(InPlayerState->PlayerNickname));
	}

	if (HPValueText)
	{
		HPValueText->SetText(FText::AsNumber(FMath::RoundToInt(InPlayerState->GetHP())));
	}

	if (LifeValueText)
	{
		LifeValueText->SetText(FText::AsNumber(5));
	}
	
	if (!BoundPlayerState->OnHPChanged.IsAlreadyBound(this, &UPlayerStatus::UpdateHP))
	{
		BoundPlayerState->OnHPChanged.AddDynamic(this, &UPlayerStatus::UpdateHP);
	}
	
	UpdateHP(BoundPlayerState->GetHP());
}
