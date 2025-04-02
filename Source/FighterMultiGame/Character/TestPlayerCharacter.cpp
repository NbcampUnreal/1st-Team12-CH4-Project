// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TestPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"





ATestPlayerCharacter::ATestPlayerCharacter()
{
	CurrentHP = MaxHP = 100.0f;
	BaseAttackDamage = 10.0f;
	KnockbackMultiplier = 1000.0f;
	ComboResetTime = 1.0f;
}

void ATestPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CheckNetwork();
	GetWorldTimerManager().SetTimer(TestAttackTimerHandle, this, &ATestPlayerCharacter::ExecuteAttack, 1.0f, true);
}

void ATestPlayerCharacter::CheckNetwork()
{
	ENetMode NetMode = GetNetMode();

	FString NetModeStr;
	switch (NetMode)
	{
	case NM_Standalone:
		NetModeStr = "Standalone";
		break;
	case NM_ListenServer:
		NetModeStr = "ListenServer";
		break;
	case NM_DedicatedServer:
		NetModeStr = "DedicatedServer";
		break;
	case NM_Client:
		NetModeStr = "Client";
		break;
	default:
		NetModeStr = "Unknown";
		break;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("[NetMode] 현재 네트워크 모드: %s"), *NetModeStr));
	}

}


//데미지처리
void ATestPlayerCharacter::ApplyDamage(AActor* Damager, float DamageAmount)
{
	const FString Context = HasAuthority() ? TEXT("서버") : TEXT("클라이언트");

	UE_LOG(LogTemp, Warning, TEXT("[ApplyDamage] 호출 위치: %s | 데미지: %.1f"), *Context, DamageAmount);

	if (HasAuthority())
	{
		Server_ApplyDamage(Damager, DamageAmount);
	}
}

void ATestPlayerCharacter::Server_ApplyDamage_Implementation(AActor* Damager, float DamageAmount)
{
	CurrentHP -= DamageAmount;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);

	UE_LOG(LogTemp, Log, TEXT("받은 데미지: %.1f | 현재 체력: %.1f"), DamageAmount, CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(5.0f);
	}
	
}

//넉백처리
void ATestPlayerCharacter::ApplyKnockback(FVector Direction, float Force)
{
	if (HasAuthority())
	{
		Multicast_PlayKnockback(Direction, Force);
	}
}

void ATestPlayerCharacter::Multicast_PlayKnockback_Implementation(FVector Direction, float Force)
{
	LaunchCharacter(Direction * Force, true, true);
}

//공격실행
void ATestPlayerCharacter::ExecuteAttack()
{
	if (!HasAuthority())
	{
		Server_RequestAttack();
		return;
	}

	CheckCombo();
	ApplyHitbox();
	Multicast_PlayAttackAnim();
}

void ATestPlayerCharacter::Server_RequestAttack_Implementation()
{
	ExecuteAttack();
}

void ATestPlayerCharacter::Multicast_PlayAttackAnim_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}
void ATestPlayerCharacter::CheckCombo()
{
	ComboCount++;
	bIsCombo = true;

	GetWorldTimerManager().ClearTimer(ComboResetHandle);
	GetWorldTimerManager().SetTimer(ComboResetHandle, this, &ATestPlayerCharacter::ResetCombo, ComboResetTime, false);
}

void ATestPlayerCharacter::ResetCombo()
{
	ComboCount = 0;
	bIsCombo = false;
}

//히트박스 , 피격판정 함수
void ATestPlayerCharacter::ApplyHitbox()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100.f;
	FVector End = Start + GetActorForwardVector() * 150.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

	if (bHit && Hit.GetActor())
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor->ActorHasTag("Enemy"))
		{
			UE_LOG(LogTemp, Warning, TEXT("[테스트 공격] Enemy 적중: %s"), *HitActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[테스트 공격] 적이 아님: %s"), *HitActor->GetName());
		}

		if (IDamageableInterface* Target = Cast<IDamageableInterface>(HitActor))
		{
			Target->ApplyDamage(this, BaseAttackDamage);

			FVector KnockDir = HitActor->GetActorLocation() - GetActorLocation();
			KnockDir.Normalize();

			float KnockForce = BaseAttackDamage * KnockbackMultiplier * 0.01f;

			Target->ApplyKnockback(KnockDir, KnockForce);
		}
	}

	//테스트용
#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 1.0f, 0, 2.0f);
#endif
}


