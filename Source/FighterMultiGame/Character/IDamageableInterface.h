// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamageableInterface.generated.h"

/**
 * 
 */
 
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
 GENERATED_BODY()
};
 
 
class FIGHTERMULTIGAME_API IDamageableInterface
{
	GENERATED_BODY()
public:
    virtual void ApplyDamage(AActor* Damager, float DamageAmount) = 0;
    virtual void ApplyKnockback(FVector Direction, float Force) = 0;

};
