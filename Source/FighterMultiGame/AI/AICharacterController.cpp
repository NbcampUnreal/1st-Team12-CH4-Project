#include "AICharacterController.h"
#include "AICharacter.h"
#include "Navigation/PathFollowingComponent.h"

void AAICharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void AAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}
