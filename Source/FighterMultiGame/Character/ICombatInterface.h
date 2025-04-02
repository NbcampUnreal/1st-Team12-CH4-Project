// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};
class FIGHTERMULTIGAME_API ICombatInterface
{
	GENERATED_BODY()
public:
	virtual void ExecuteAttack() = 0;
	virtual void CheckCombo() = 0;
	virtual void ApplyHitbox() = 0;

};
