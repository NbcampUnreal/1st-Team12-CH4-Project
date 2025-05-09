// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FMG_PlayerState.generated.h"

/**
 * 
 */


UCLASS()
class FIGHTERMULTIGAME_API AFMG_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerIndexChanged, AFMG_PlayerState*, State);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float, NewHP);

	
	UFUNCTION()
	void OnRep_PlayerIndex(); 
	UPROPERTY(BlueprintReadWrite)
	FOnPlayerIndexChanged OnPlayerIndexChanged;
	UPROPERTY(Replicated, BlueprintReadWrite,Category="Status")
	int32 PlayerIndex = -1;

	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHP, BlueprintReadWrite,Category="Status")
	float CurrentHP = 100.f;
	void SetHP(float NewHP);
	float GetHP() { return CurrentHP; }
	
	UFUNCTION()
	void OnRep_CurrentHP();
	UPROPERTY(BlueprintAssignable, Category="Status")
	FOnHPChanged OnHPChanged;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Status")
	FString PlayerNickname;

	// 플레이어가 선택한 캐릭터
	UPROPERTY(ReplicatedUsing = OnRep_SelectCharacter, BlueprintReadWrite, Category = "Character Select")
	int32 SelectedCharacterIndex = -1;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Character Select")
	bool bSelectConfirmed = false;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_SelectCharacter();

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
