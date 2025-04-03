// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FMGPlayerController.generated.h"


class UInputMappingContext; 
class UInputAction;


UCLASS()
class FIGHTERMULTIGAME_API AFMGPlayerController : public APlayerController
{
	GENERATED_BODY()

	
public:
	AFMGPlayerController();
	
	virtual void BeginPlay() override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* MoveActionInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* JumpActionInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* BasicAttackInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SpecialAttackInput;
};
