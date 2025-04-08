// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/ICombatInterface.h"
#include "Character/IDamageableInterface.h"
#include "Components/BoxComponent.h"
#include "BaseCharacter.generated.h"



UCLASS()
class FIGHTERMULTIGAME_API ABaseCharacter : public ACharacter, public IDamageableInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:	
	// 전투 관련 인터페이스
	virtual void ApplyDamage(AActor* Damager, float DamageAmount) override;
	virtual void ApplyKnockback(FVector Direction, float Force) override;
	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAttack() override;
	virtual void AddCombo() override;
	virtual void ApplyHitbox() override;

	
	//전투관련 유틸함수
	void ResetKnockback();
	void DisplayHitActorHP();
	UFUNCTION()
	void OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent,
							  AActor* OtherActor,
							  UPrimitiveComponent* OtherComp,
							  int32 OtherBodyIndex,
							  bool bFromSweep,
							  const FHitResult& SweepResult);
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyDamage(AActor* Damager, float DamageAmount);


	
	
private:
	int32 Power;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	float SpecialAtkCool;
	float SpecialAtkCurrCool;



	
public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bTrackCam;


	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, EditAnywhere, BlueprintReadWrite, Category="Status")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float MaxHP;

	UFUNCTION()
	void OnRep_CurrentHP();

	UFUNCTION(BlueprintCallable)
	void SetHP(float NewHP);

	UFUNCTION(BlueprintCallable)
	float GetHP() const { return CurrentHP; }

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float, CurrentHP);
	UPROPERTY(BlueprintAssignable, Category="Status")
	FOnHPChanged OnHPChanged;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	
	//Ui관련변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	UTexture2D* CharacterIcon;
	UTexture2D* GetChracterIcon() const{ return CharacterIcon; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	FString CharacterName;
	FString GetCharacterName() const { return CharacterName; }


//전투관련변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSet<AActor*> HitActors;
	float MaxKnockbackGauge = 100.f;
	float CurrKnockbackGauge = 0.f;

	bool bIsKnockback = false;

	bool bIsCombo = false;
	int32 ComboCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* PlayerHitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float KnockbackMultiplier = 1000.0f;
	

	float Speed = 500.0f;

};



inline void ABaseCharacter::ResetKnockback()
{
	bIsKnockback = false;
}

inline void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bIsKnockback = false;
}
