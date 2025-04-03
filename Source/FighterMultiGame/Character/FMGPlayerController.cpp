// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FMGPlayerController.h"
#include "EnhancedInputSubsystems.h"

AFMGPlayerController::AFMGPlayerController() : InputMappingContext(nullptr),
	  MoveActionInput(nullptr),
	  JumpActionInput(nullptr),
	  BasicAttackInput(nullptr),
	  SpecialAttackInput(nullptr)
{
}

void AFMGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				// Subsystem을 통해 우리가 할당한 IMC를 활성화
				// 우선순위(Priority)는 0이 가장 높은 우선순위
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
