// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/PlayerStatus.h"
#include "Components/HorizontalBox.h"
#include "StatusView.generated.h"



UCLASS()
class FIGHTERMULTIGAME_API UStatusView : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayerIndexChangedHandler(AFMG_PlayerState* ChangedState);


	UFUNCTION(BlueprintCallable)
	void RefreshPlayerList();

	UFUNCTION(BlueprintCallable)
	void UpdateUI();
	

protected:
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* PlayerStatusContainer;

	
	UPROPERTY()
	TArray<UPlayerStatus*> PlayerStatusList;
	
	UPROPERTY(meta = (BindWidget))
	UPlayerStatus* WBP_PlayerStatus_1;
	UPROPERTY(meta = (BindWidget))
	UPlayerStatus* WBP_PlayerStatus_2;
	UPROPERTY(meta = (BindWidget))
	UPlayerStatus* WBP_PlayerStatus_3;
	UPROPERTY(meta = (BindWidget))
	UPlayerStatus* WBP_PlayerStatus_4;

private:
	void InitDelegate();
};
