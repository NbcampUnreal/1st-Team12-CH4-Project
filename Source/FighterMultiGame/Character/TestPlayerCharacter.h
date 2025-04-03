#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "TestPlayerCharacter.generated.h"


class UCameraComponent;
struct FInputActionValue;


UCLASS()
class FIGHTERMULTIGAME_API ATestPlayerCharacter : public ATP_ThirdPersonCharacter
{
    GENERATED_BODY()

public:
    ATestPlayerCharacter();


    virtual void PossessedBy(AController* NewController) override;
    void FixCamera();
    
    virtual void OnRep_Controller() override;
protected:
    virtual void BeginPlay() override;

    

    //
    //
    // protected:
    // virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // UFUNCTION()
    // void Move(const FInputActionValue& value);
    // UFUNCTION()
    // void StartJump(const FInputActionValue& value);
    // UFUNCTION()
    // void StopJump(const FInputActionValue& value);
    // UFUNCTION()
    // void BasicAttack(const FInputActionValue& value);
    // UFUNCTION()
    // void SpecialAttack(const FInputActionValue& value);


    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
    float ComboResetTime = 1.0f;

    FTimerHandle ComboResetHandle;



//테스트용
    FTimerHandle TestAttackTimerHandle;

    void CheckNetwork();

    

    
};

