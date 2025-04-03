// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
	
}

void ATP_ThirdPersonCharacter::ApplyKnockback(FVector Direction, float Force)
{
	
}

void ATP_ThirdPersonCharacter::ExecuteAttack()
{
}

void ATP_ThirdPersonCharacter::CheckCombo()
{
}

void ATP_ThirdPersonCharacter::ApplyHitbox()
{
	
}

void ATP_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(FVector::RightVector, MovementVector.X);
	}
}
