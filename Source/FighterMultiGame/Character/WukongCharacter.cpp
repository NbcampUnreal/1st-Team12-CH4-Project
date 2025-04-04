// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WukongCharacter.h"

void AWukongCharacter::OnSpecialAttack(AActor* OtherActor)
{
	if (IDamageableInterface* Target = Cast<IDamageableInterface>(OtherActor))
	{
		Target->ApplyDamage(this, SpecialAttackDamage);

		FVector KnockDir = OtherActor->GetActorLocation() - GetActorLocation();
		KnockDir.Z = 0.f; 
		KnockDir.Normalize();

		KnockDir.Z = 0.3f; 
		KnockDir.Normalize();

		float KnockForce = FMath::Clamp(SpecialAttackDamage * KnockbackMultiplier * 0.1f, 300.f, 800.f);

		Target->ApplyKnockback(KnockDir, KnockForce);
	}
}
