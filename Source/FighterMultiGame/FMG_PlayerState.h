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
	
	UFUNCTION()
	void OnRep_PlayerIndex(); 
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerIndexChanged, AFMG_PlayerState*, State);

	UPROPERTY(BlueprintReadWrite)
	FOnPlayerIndexChanged OnPlayerIndexChanged;
	
	UPROPERTY(Replicated, BlueprintReadWrite,Category="Status")
	int32 PlayerIndex = -1;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHP, BlueprintReadWrite,Category="Status")
	float CurrentHP = 100.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Status")
	FString PlayerNickname;

	UFUNCTION()
	void OnRep_CurrentHP();

	void SetHP(float NewHP);
	float GetHP() { return CurrentHP; }
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float, NewHP);
	UPROPERTY(BlueprintAssignable, Category="Status")
	FOnHPChanged OnHPChanged;
	

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
