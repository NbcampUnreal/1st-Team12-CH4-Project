#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/FMGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Character/ICombatInterface.h"
#include "Character/IDamageableInterface.h"
#include "Components/BoxComponent.h"
#include "TestPlayerCharacter.generated.h"


class UCameraComponent;
struct FInputActionValue;


UCLASS()
class FIGHTERMULTIGAME_API ATestPlayerCharacter : public ABaseCharacter, public IDamageableInterface, public ICombatInterface
{
    GENERATED_BODY()

public:
    ATestPlayerCharacter();

    virtual void ApplyDamage(AActor* Damager, float DamageAmount) override;
    virtual void ApplyKnockback(FVector Direction, float Force) override;

    virtual void ExecuteAttack() override;
    virtual void CheckCombo() override;
    virtual void ApplyHitbox() override;

    UFUNCTION()
    void OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex,
                              bool bFromSweep,
                              const FHitResult& SweepResult);
    
    void ResetCombo();

    virtual void PossessedBy(AController* NewController) override;
    void FixCamera();

protected:
    virtual void BeginPlay() override;

    UFUNCTION(Server, Reliable)
    void Server_ApplyDamage(AActor* Damager, float DamageAmount);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayKnockback(FVector KnockDirection, float Force);

    UFUNCTION(Server, Reliable)
    void Server_RequestAttack();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayAttackAnim();

    
    virtual void OnRep_Controller() override;

    //

    protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    UFUNCTION()
    void Move(const FInputActionValue& value);
    UFUNCTION()
    void StartJump(const FInputActionValue& value);
    UFUNCTION()
    void StopJump(const FInputActionValue& value);
    UFUNCTION()
    void BasicAttack(const FInputActionValue& value);
    UFUNCTION()
    void SpecialAttack(const FInputActionValue& value);


    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class UBoxComponent* PlayerHitBox;
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
    float BaseAttackDamage = 10.0f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
    float KnockbackMultiplier = 1000.0f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

    float CurrentHP = 100.0f;
    float MaxHP = 100.0f;
    
    float Speed = 500.0f;

    float MaxKnockbackGauge = 100.f;
    float CurrKnockbackGauge = 0.f;

    bool bIsCombo = false;
    int32 ComboCount = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
    float ComboResetTime = 1.0f;

    FTimerHandle ComboResetHandle;



//테스트용
    FTimerHandle TestAttackTimerHandle;

    void CheckNetwork();

    

    
};

