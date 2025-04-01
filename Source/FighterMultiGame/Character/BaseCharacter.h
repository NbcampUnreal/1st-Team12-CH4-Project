// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS(Abstract)
class FIGHTERMULTIGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BaseAttack() PURE_VIRTUAL(ABaseCharacter::BaseAttack);
	virtual void SpecialAttack() PURE_VIRTUAL(ABaseCharacter::SpecialAttack);

	virtual void Move() PURE_VIRTUAL(ABaseCharacter::Move);
	virtual void Jump() PURE_VIRTUAL(ABaseCharacter::Jump);

	//void GetItem();
	virtual void Damaged() PURE_VIRTUAL(ABaseCharacter::Damaged);
	virtual void Die() PURE_VIRTUAL(ABaseCharacter::Die);
	
private:
	int32 Hp;
	int32 Power;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	float SpecialAtkCool;
	float SpecialAtkCurrCool;
};
