#include "Player/Behaviours/PlayerCharacterClassBase.h"

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerCharacterControllerBase.h"

void UPlayerCharacterClassBase::Initialize(const FCharacterClassInitializationInfo& info)
{
	_controller = info.Controller;
	_character = info.Character;
	_animInstance = Cast<UPlayerCharacterAnimInstanceBase>(_character->GetMesh()->GetAnimInstance());
	_movementSpeed = info.MovementSpeed;

	_animInstance->SetFalling(_character->bClientWasFalling);
}

void UPlayerCharacterClassBase::Tick(float deltaSeconds)
{
	if (!_isMovingEnabled)
	{
		_animInstance->SetMovementSpeed(0.0f);
		
		return;
	}
	
	const FVector directionVector = FVector(_directionVector, 0.0f);

	if (_controller.IsValid() &&
		_directionVector != FVector2D::ZeroVector &&
		!_animInstance->IsFalling())
	{
		_controller->SetControlRotation(directionVector.Rotation());
	}
	
	_character->AddMovementInput(directionVector.GetClampedToMaxSize(1), _movementSpeed);

	_animInstance->SetMovementSpeed(directionVector.Length());
}

void UPlayerCharacterClassBase::OnFalling()
{
	_animInstance->SetFalling(true);
}

void UPlayerCharacterClassBase::OnLanded()
{
	_animInstance->SetJumping(false);
	_animInstance->SetFalling(false);
}

bool UPlayerCharacterClassBase::SetMovementDirection(const FVector2D& directionVector)
{
	if (_directionVector == directionVector) return false;
	
	_directionVector = directionVector;

	return true;
}

FVector2D UPlayerCharacterClassBase::GetMovementDirection() const
{
	return _directionVector;
}

bool UPlayerCharacterClassBase::Jump()
{
	if (_character->CanJump())
	{
		_character->Jump();

		_animInstance->SetJumping(true);

		return true;
	}

	return false;
}

