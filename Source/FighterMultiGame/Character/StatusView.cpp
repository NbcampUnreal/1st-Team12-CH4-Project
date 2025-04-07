// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/StatusView.h"
#include "FMG_PlayerState.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

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
	UE_LOG(LogTemp, Warning, TEXT("리프래쉬 적용") );
	
	RefreshPlayerList();
}

void UStatusView::RefreshPlayerList()
{
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), AllCharacters);

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
				StatusWidget->SetPlayerInfo(Character);

				if (!FMGState->OnHPChanged.IsAlreadyBound(StatusWidget, &UPlayerStatus::UpdateHP))
				{
					FMGState->OnHPChanged.AddDynamic(StatusWidget, &UPlayerStatus::UpdateHP);
				}
				StatusWidget->UpdateHP(FMGState->GetHP());
			}
		}
	}
}
