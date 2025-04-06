// Fill out your copyright notice in the Description page of Project Settings.


#include "FMG_GameState.h"

TArray<APlayerState*> AFMG_GameState::GetAllPlayerStates() const
{
	return PlayerArray;
}
