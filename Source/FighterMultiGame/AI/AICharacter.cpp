#include "AICharacter.h"
#include "AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAICharacter::AAICharacter()
{
	AIControllerClass = AAICharacterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

float AAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	LastAttacker = DamageCauser;

	if (AAICharacterController* AIController = Cast<AAICharacterController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
		{
			BB->SetValueAsObject("LastAttacker", DamageCauser);
			BB->SetValueAsBool("Damaged", true);
		}
	}

	return ActualDamage;
}

void AAICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AAICharacterController* AIController = Cast<AAICharacterController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
		{
			AActor* LandedActor = Hit.GetActor();
			if (LandedActor)
			{
				BB->SetValueAsObject(FName("LastPlatform"), LandedActor);
			}
		}
	}
}
