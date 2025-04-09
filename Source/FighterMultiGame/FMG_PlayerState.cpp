// Fill out your copyright notice in the Description page of Project Settings.


#include "FMG_PlayerState.h"
#include "Net/UnrealNetwork.h"


void AFMG_PlayerState::OnRep_PlayerIndex()
{
	OnPlayerIndexChanged.Broadcast(this);
}

//ui넣어주기
void AFMG_PlayerState::OnRep_CurrentHP()
{
	UE_LOG(LogTemp, Warning, TEXT("[OnRep_CurrentHP] 호출: %.1f"), CurrentHP);
	OnHPChanged.Broadcast(CurrentHP);
}

void AFMG_PlayerState::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP, 0.f, 100.f);
}

void AFMG_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFMG_PlayerState, CurrentHP);
	DOREPLIFETIME(AFMG_PlayerState, PlayerIndex);

	DOREPLIFETIME(AFMG_PlayerState, SelectedCharacterIndex);
	DOREPLIFETIME(AFMG_PlayerState, bSelectConfirmed);
}
