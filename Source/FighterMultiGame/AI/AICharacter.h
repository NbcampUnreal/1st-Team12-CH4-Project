#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AICharacter.generated.h"


UCLASS()
class FIGHTERMULTIGAME_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAICharacter();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LastAttacker;

	virtual void Landed(const FHitResult& Hit) override;
};
