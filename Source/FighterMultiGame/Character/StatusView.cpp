// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StatusView.h"
#include "FMG_PlayerState.h"

#include "GameFramework/GameStateBase.h"

void UStatusView::NativeConstruct()
{
	Super::NativeConstruct();
	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, this, &UStatusView::RefreshPlayerList, 0.8f, false);
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

void UStatusView::OnPlayerIndexChangedHandler(AFMG_PlayerState* ChangedState)
{
	RefreshPlayerList();
}

void UStatusView::RefreshPlayerList()
{
	TArray<APlayerState*> PlayerStates = GetWorld()->GetGameState()->PlayerArray;

	for (APlayerState* State : PlayerStates)
	{
		if (AFMG_PlayerState* FMGState = Cast<AFMG_PlayerState>(State))
		{
			int32 Index = FMGState->PlayerIndex;
			if (Index < 0 || Index > 3) continue;

			UPlayerStatus* StatusWidget = nullptr;

			switch (Index)
			{
			case 0: StatusWidget = WBP_PlayerStatus_1; break;
			case 1: StatusWidget = WBP_PlayerStatus_2; break;
			case 2: StatusWidget = WBP_PlayerStatus_3; break;
			case 3: StatusWidget = WBP_PlayerStatus_4; break;
			}

			if (StatusWidget)
			{
				StatusWidget->SetPlayerInfo(FMGState);
			}
		}
	}
}
