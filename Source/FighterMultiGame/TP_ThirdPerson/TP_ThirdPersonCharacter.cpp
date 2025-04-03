// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Components/BoxComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	JumpMaxCount = 2;
	
	ConstructorHelpers::FObjectFinder<UInputMappingContext> tempIMC(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/SeolBeen/ThirdPerson/Input/IMC_Default.IMC_Default'"));
	if (tempIMC.Succeeded())
	{
		DefaultMappingContext = tempIMC.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> tempIA_Move(TEXT("/Script/EnhancedInput.InputAction'/Game/SeolBeen/ThirdPerson/Input/Actions/IA_Move.IA_Move'"));
	if (tempIA_Move.Succeeded())
	{
		MoveAction = tempIA_Move.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> tempIA_Jump(TEXT("/Script/EnhancedInput.InputAction'/Game/SeolBeen/ThirdPerson/Input/Actions/IA_Jump.IA_Jump'"));
	if (tempIA_Jump.Succeeded())
	{
		JumpAction = tempIA_Jump.Object;
	}

	/*공격 쪽 구현은 아직 미정으로 일단 InputAction만 생성한 상태
	 *아직 함수 바인딩은 하지 않음.
	 */
	ConstructorHelpers::FObjectFinder<UInputAction> tempIA_BasicAttack(TEXT("/Script/EnhancedInput.InputAction'/Game/SeolBeen/ThirdPerson/Input/Actions/IA_BasicAttack.IA_BasicAttack'"));
	if (tempIA_BasicAttack.Succeeded())
	{
		BasicAttackAction = tempIA_BasicAttack.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> tempIA_SpecialAttack(TEXT("/Script/EnhancedInput.InputAction'/Game/SeolBeen/ThirdPerson/Input/Actions/IA_SpecialAttack.IA_SpecialAttack'"));
	if (tempIA_SpecialAttack.Succeeded())
	{
		SpecialAttackAction = tempIA_SpecialAttack.Object;
	}
}

void ATP_ThirdPersonCharacter::BeginPlay()
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
		PlayerHitBox->OnComponentBeginOverlap.AddDynamic(this, &ATP_ThirdPersonCharacter::OnHitBoxOverlapBegin);
		PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본 비활성화
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATP_ThirdPersonCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Move);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATP_ThirdPersonCharacter::ApplyDamage(AActor* Damager, float DamageAmount)
{
	const FString Context = HasAuthority() ? TEXT("서버") : TEXT("클라이언트");

	UE_LOG(LogTemp, Warning, TEXT("[ApplyDamage] 호출 위치: %s | 데미지: %.1f"), *Context, DamageAmount);

	if (HasAuthority())
	{
		Server_ApplyDamage(Damager, DamageAmount);
	}
	
}

void ATP_ThirdPersonCharacter::ApplyKnockback(FVector Direction, float Force)
{
	if (HasAuthority())
	{
		bIsKnockback = true;
		FTimerHandle KnockbackResetHandle;
		GetWorldTimerManager().SetTimer(
			KnockbackResetHandle,
			this,
			&ATP_ThirdPersonCharacter::ResetKnockback,
			0.3f,
			false
		);
		
		Multicast_PlayKnockback(Direction, Force);
	}
}

void ATP_ThirdPersonCharacter::ExecuteAttack()
{
	if (bIsKnockback) return; // 넉백 중에는 공격 금지
	
	if (!HasAuthority())
	{
		Server_RequestAttack();
		return;
	}

	AddCombo();
	ApplyHitbox();
	Multicast_PlayAttackAnim();
}

void ATP_ThirdPersonCharacter::AddCombo()
{
	ComboCount++;
	bIsCombo = true;
}

void ATP_ThirdPersonCharacter::ApplyHitbox()
{
	if (!PlayerHitBox) return;

	HitActors.Empty();
	PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FTimerHandle DisableHandle;
	GetWorldTimerManager().SetTimer(DisableHandle, [this]()
	{
		if (PlayerHitBox)
		{
			PlayerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}, 0.1f, false);
}

void ATP_ThirdPersonCharacter::Server_ApplyDamage_Implementation(AActor* Damager, float DamageAmount)
{
	CurrentHP -= DamageAmount;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);

	UE_LOG(LogTemp, Log, TEXT("받은 데미지: %.1f | 현재 체력: %.1f"), DamageAmount, CurrentHP);

	if (CurrentHP <= 0.0f)
	{
		GetMesh()->SetSimulatePhysics(true);
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(5.0f);
	}
	
}

void ATP_ThirdPersonCharacter::Multicast_PlayKnockback_Implementation(FVector KnockDirection, float Force)
{
	
	LaunchCharacter(KnockDirection * Force, true, true);
}

void ATP_ThirdPersonCharacter::Server_RequestAttack_Implementation()
{
	ExecuteAttack();
}

void ATP_ThirdPersonCharacter::Multicast_PlayAttackAnim_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void ATP_ThirdPersonCharacter::OnHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && !HitActors.Contains(OtherActor))
	{
		HitActors.Add(OtherActor);
		if (IDamageableInterface* Target = Cast<IDamageableInterface>(OtherActor))
		{
			Target->ApplyDamage(this, BaseAttackDamage);

			
			FVector KnockDir = OtherActor->GetActorLocation() - GetActorLocation();
			KnockDir.Z = 0.f; 
			KnockDir.Normalize();

			KnockDir.Z = 0.3f; 
			KnockDir.Normalize();

			float KnockForce = FMath::Clamp(BaseAttackDamage * KnockbackMultiplier * 0.01f, 300.f, 800.f);


			Target->ApplyKnockback(KnockDir, KnockForce);
		}
	}
}

void ATP_ThirdPersonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ATP_ThirdPersonCharacter::ResetCombo()
{
	ComboCount = 0;
	bIsCombo = false;
}

void ATP_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(FVector::LeftVector, MovementVector.X);
	}
}
