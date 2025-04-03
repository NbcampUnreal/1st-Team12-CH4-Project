// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TestPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"


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
		PlayerHitBox->OnComponentBeginOverlap.AddDynamic(this, &ATestPlayerCharacter::OnHitBoxOverlapBegin);
		PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본 비활성화
	}
	
	CheckNetwork();

	FTimerHandle CameraTimerHandle;
	GetWorldTimerManager().SetTimer(CameraTimerHandle, this, &ATestPlayerCharacter::FixCamera, 0.5f, false);

	GetWorldTimerManager().SetTimer(TestAttackTimerHandle, this, &ATestPlayerCharacter::ExecuteAttack, 1.0f, true);
}

void ATestPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	FTimerHandle DelayFixCameraHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayFixCameraHandle, this, &ATestPlayerCharacter::FixCamera, 0.5f, false);

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

void ATestPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	FixCamera();
}

void ATestPlayerCharacter::FixCamera()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalController()) 
	{
		TArray<AActor*> FoundCameras;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

		if (FoundCameras.Num() > 0)
		{
			PC->SetViewTargetWithBlend(FoundCameras[0], 0.5f);
			UE_LOG(LogTemp, Log, TEXT("고정 카메라로 변경 완료"));
		}
	}

}

//히트박스 , 피격판정 함수
void ATestPlayerCharacter::ApplyHitbox()
{
	if (!PlayerHitBox) return;

	PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FTimerHandle DisableHandle;
	GetWorldTimerManager().SetTimer(DisableHandle, [this]()
	{
		if (PlayerHitBox)
		{
			PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}, 0.2f, false);
}

void ATestPlayerCharacter::OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->ActorHasTag("Enemy")) 
		{
			UE_LOG(LogTemp, Warning, TEXT("히트받스에Enemy 적중: %s"), *OtherActor->GetName());
		}

		if (IDamageableInterface* Target = Cast<IDamageableInterface>(OtherActor))
		{
			Target->ApplyDamage(this, BaseAttackDamage);

			FVector KnockDir = OtherActor->GetActorLocation() - GetActorLocation();
			KnockDir.Normalize();

			float KnockForce = BaseAttackDamage * KnockbackMultiplier * 0.01f;

			Target->ApplyKnockback(KnockDir, KnockForce);
		}
	}
}


void ATestPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AFMGPlayerController* PlayerController = Cast<AFMGPlayerController>(GetController()))
		{
			if (PlayerController->MoveActionInput)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveActionInput,
					ETriggerEvent::Triggered,
					this,
					&ATestPlayerCharacter::Move
				);
			}
            
			if (PlayerController->JumpActionInput)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpActionInput,
					ETriggerEvent::Triggered,
					this,
					&ATestPlayerCharacter::StartJump
				);
                
				EnhancedInput->BindAction(
					PlayerController->JumpActionInput,
					ETriggerEvent::Completed,
					this,
					&ATestPlayerCharacter::StopJump
				);
			}
			if (PlayerController->BasicAttackInput)
			{
				EnhancedInput->BindAction(
					PlayerController->BasicAttackInput,
					ETriggerEvent::Started,
					this,
					&ATestPlayerCharacter::BasicAttack
				);
			}
			if (PlayerController->SpecialAttackInput)
			{
				EnhancedInput->BindAction(
					PlayerController->SpecialAttackInput,
					ETriggerEvent::Started,
					this,
					&ATestPlayerCharacter::SpecialAttack
				);
			}
		}
	}
}

void ATestPlayerCharacter::Move(const FInputActionValue& value)
{
	const float InputAxis = value.Get<float>();

	if (Controller && InputAxis != 0.0f)
	{
		AddMovementInput(FVector::YAxisVector, InputAxis); 

		
		const float TargetYaw = (InputAxis > 0.f) ? 90.f : -90.f;
		SetActorRotation(FRotator(0.f, TargetYaw, 0.f));
	}
}

void ATestPlayerCharacter::StartJump(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("점프 키 입력"));
	Jump();
}

void ATestPlayerCharacter::StopJump(const FInputActionValue& value)
{
	StopJumping();
}

void ATestPlayerCharacter::BasicAttack(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("일반공격 키 입력"));
	if (!HasAuthority())
	{
		Server_RequestAttack();
		return;
	}

	ExecuteAttack();
}

void ATestPlayerCharacter::SpecialAttack(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("특수공격 키 입력"));

}
