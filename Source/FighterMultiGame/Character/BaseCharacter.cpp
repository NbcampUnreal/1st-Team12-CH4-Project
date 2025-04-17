// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "DebugHelper.h"
#include "KchTestGameMode.h"
#include "Components/BoxComponent.h"
#include "Character/StatusView.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bTrackCam = true;

	CharacterIcon = nullptr;
	CurrentHP = MaxHP = 100.f;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	for (UActorComponent* Comp : GetComponents())
	{
		if (UBoxComponent* Box = Cast<UBoxComponent>(Comp))
		{
			if (Box->GetName() == TEXT("HitBox"))
			{
				PlayerHitBox = Box;
				break;
			}
		}
	}
	if (PlayerHitBox)
	{
		PlayerHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnHitBoxOverlapBegin);
		PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	}

	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const 
{ 	
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
	DOREPLIFETIME(ABaseCharacter, bTrackCam);
	DOREPLIFETIME(ABaseCharacter, CurrentHP);
}

void ABaseCharacter::Multicast_PlayDeathMontage_Implementation()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
}

void ABaseCharacter::Multicast_PlayHitSound_Implementation(FVector Location)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Location);
	}
}

void ABaseCharacter::Multicast_PlayHitEffect_Implementation(FVector Location)
{
	if (HitEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitEffect,
			Location,
			FRotator::ZeroRotator,
			FVector(2.0f)	
		);
	}
}


void ABaseCharacter::DisplayHitActorHP()
{
	for (AActor* HitActor : HitActors)
	{
		if (ABaseCharacter* Target = Cast<ABaseCharacter>(HitActor))
		{
				UE_LOG(LogTemp, Log, TEXT("[맞은놈] %s - HP: %.1f"), *Target->CharacterName, Target->GetHP());
		}
	}
}

void ABaseCharacter::OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || HitActors.Contains(OtherActor))
	{
		return;
	}

	if (Cast<ABaseCharacter>(OtherActor) == this)
	{
		return;
	}

	if (OtherActor->GetInstigatorController() == GetController())
	{
		return;
	}

	HitActors.Add(OtherActor);

	if (IDamageableInterface* Target = Cast<IDamageableInterface>(OtherActor))
	{
		if (HasAuthority())
		{
			Multicast_PlayHitEffect(OtherActor->GetActorLocation());
		}

		Target->ApplyDamage(this, BaseAttackDamage);
		DisplayHitActorHP();

		FVector KnockDir = OtherActor->GetActorLocation() - GetActorLocation();
		KnockDir.Z = 0.3f;
		KnockDir.Normalize();

		float KnockForce = FMath::Clamp(BaseAttackDamage * KnockbackMultiplier * 0.01f, 300.f, 800.f);
		Target->ApplyKnockback(KnockDir, KnockForce);
	}
}

void ABaseCharacter::ApplyDamage(AActor* Damager, float DamageAmount)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
			FString::Printf(TEXT("베이스몬스터 데미지적용 %.1f | %s"),
			DamageAmount,
			*DebugHelper::GetNetModeName(GetWorld())));
	}
	Server_ApplyDamage(Damager, DamageAmount);
	
}

void ABaseCharacter::ApplyKnockback(FVector Direction, float Force)
{
	bIsKnockback = true;
	LaunchCharacter(Direction * Force, true, true);
}

void ABaseCharacter::Server_ApplyDamage_Implementation(AActor* Damager, float DamageAmount)
{
	SetHP(CurrentHP - DamageAmount);

	if (CurrentHP <= 0.0f)
	{
		
		bIsDead = true;
		Multicast_PlayDeathMontage();
		GetCharacterMovement()->DisableMovement();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetSimulatePhysics(false);
		SetLifeSpan(1.0f);
		
		if (HasAuthority())
		{
			if (AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld()))
			{
				if (AKchTestGameMode* TestGM = Cast<AKchTestGameMode>(GM))
				{
					TestGM->CheckTeamAllDead();
				}
			}
		}
	}
}

void ABaseCharacter::OnRep_CurrentHP()
{
	OnHPChanged.Broadcast(CurrentHP);
}

void ABaseCharacter::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP, 0.f, MaxHP);
}


void ABaseCharacter::ExecuteAttack()
{
	if (bIsKnockback) return; // 넉백 중에는 공격 금지
	
	AddCombo();
	ApplyHitbox();
}

void ABaseCharacter::AddCombo()
{
	ComboCount++;
	bIsCombo = true;
}

void ABaseCharacter::ApplyHitbox()
{
	if (!PlayerHitBox) return;

	HitActors.Empty();
	PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerHitBox->IgnoreActorWhenMoving(this, true);
	PlayerHitBox->MoveIgnoreActors.Add(this);

	FTimerHandle DisableHandle;
	GetWorldTimerManager().SetTimer(DisableHandle, [this]()
	{
		if (PlayerHitBox)
		{
			PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}, 0.1f, false);
}