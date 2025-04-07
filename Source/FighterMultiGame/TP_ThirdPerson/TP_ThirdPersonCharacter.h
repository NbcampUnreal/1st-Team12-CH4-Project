// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/ICombatInterface.h"
#include "Character/IDamageableInterface.h"
#include "Character/StatusView.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TP_ThirdPersonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ATP_ThirdPersonCharacter : public ABaseCharacter, public IDamageableInterface, public ICombatInterface
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> BasicAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SpecialAttackAction;

public:
	ATP_ThirdPersonCharacter();

	virtual void BeginPlay() override;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void ApplyDamage(AActor* Damager, float DamageAmount) override;
	virtual void ApplyKnockback(FVector Direction, float Force) override;

	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAttack() override;
	virtual void AddCombo() override;
	virtual void ApplyHitbox() override;

protected:
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyDamage(AActor* Damager, float DamageAmount);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayKnockback(FVector KnockDirection, float Force);

	UFUNCTION(Server, Reliable)
	void Server_RequestAttack();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackAnim();


	UFUNCTION()
	void OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex,
	                          bool bFromSweep,
	                          const FHitResult& SweepResult);

	void ResetCombo();

	void DisplayHitActorHP();

	virtual void PossessedBy(AController* NewController) override;

	void ResetKnockback();
	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* PlayerHitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float KnockbackMultiplier = 1000.0f;
	
	float MaxHP = 100.0f;

	float Speed = 500.0f;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UStatusView> StatusViewClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSet<AActor*> HitActors;
	float MaxKnockbackGauge = 100.f;
	float CurrKnockbackGauge = 0.f;

	bool bIsKnockback = false;

	bool bIsCombo = false;
	int32 ComboCount = 0;
};

inline void ATP_ThirdPersonCharacter::ResetKnockback()
{
	bIsKnockback = false;
}

inline void ATP_ThirdPersonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bIsKnockback = false;
}
