// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StatusView.h"
#include "FMG_PlayerState.h"
#include "Character/PlayerStatus.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UStatusView::NativeConstruct()
{
	Super::NativeConstruct();
	
	InitDelegate();
	RefreshPlayerList();
}

void UStatusView::OnPlayerIndexChangedHandler(AFMG_PlayerState* ChangedState)
{
	RefreshPlayerList();
}

void UStatusView::RefreshPlayerList()
{
	PlayerStatusList.Empty();
	PlayerStatusList.SetNum(4);
	PlayerStatusList[0] = IsValid(WBP_PlayerStatus_1) ? WBP_PlayerStatus_1 : nullptr;
	PlayerStatusList[1] = IsValid(WBP_PlayerStatus_2) ? WBP_PlayerStatus_2 : nullptr;
	PlayerStatusList[2] = IsValid(WBP_PlayerStatus_3) ? WBP_PlayerStatus_3 : nullptr;
	PlayerStatusList[3] = IsValid(WBP_PlayerStatus_4) ? WBP_PlayerStatus_4 : nullptr;

	
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), AllCharacters);

	UE_LOG(LogTemp, Warning, TEXT("리프래쉬 적용") );
	for (AActor* Actor : AllCharacters)
	{
		if (ABaseCharacter* Character = Cast<ABaseCharacter>(Actor))
		{
			int32 Index = -1;

			AFMG_PlayerState* FMGState = Cast<AFMG_PlayerState>(Character->GetPlayerState());
			if (FMGState)
			{
				Index = FMGState->PlayerIndex;
			}
			else
			{
				static int32 AICount = 2;
				Index = AICount++;
			}

			if (Index < 0 || Index > 3) continue;
			if (!PlayerStatusList.IsValidIndex(Index)) continue;
			UE_LOG(LogTemp, Warning, TEXT("캐릭터: %s | Index: %d"), *Character->GetName(), Index);

			UPlayerStatus* StatusWidget = PlayerStatusList[Index];
			if (!IsValid(StatusWidget)) continue;
			StatusWidget->SetPlayerInfo(Character);
			PlayerStatusList[Index]->SetPlayerInfo(Character); 
			
		}
	}
}

void UStatusView::UpdateUI()
{

	for (int32 i = 0; i < PlayerStatusList.Num(); ++i)
	{
		UPlayerStatus* playerstatus = PlayerStatusList[i];

		if (!IsValid(playerstatus)) continue;

		ABaseCharacter* character = playerstatus->GetCharacter();

		if (!IsValid(character)) continue;

		playerstatus->UpdateHP(character->GetHP());
	}
	
	// for (UPlayerStatus* playerstatus : PlayerStatusList)
	// {
	// 	if (!playerstatus) continue;
	//
	// 	ABaseCharacter* character = playerstatus->GetCharacter();
	// 	if (!character) continue;
	//
	// 	float tempHp = character->GetHP();
	// 	playerstatus->UpdateHP(tempHp);
	// }
}

void UStatusView::InitDelegate()
{
	for (APlayerState* State : GetWorld()->GetGameState()->PlayerArray)
	{
		if (AFMG_PlayerState* FMGState = Cast<AFMG_PlayerState>(State))
		{
			if (!FMGState->OnPlayerIndexChanged.IsAlreadyBound(this, &UStatusView::OnPlayerIndexChangedHandler))
			{
				FMGState->OnPlayerIndexChanged.AddDynamic(this, &UStatusView::OnPlayerIndexChangedHandler);
			}
		}
	}
}
