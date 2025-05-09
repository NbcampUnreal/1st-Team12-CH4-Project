// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "WukongCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTERMULTIGAME_API AWukongCharacter : public ATP_ThirdPersonCharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void OnSpecialAttack(AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SpecialAttackDamage = 15.0f;
};
