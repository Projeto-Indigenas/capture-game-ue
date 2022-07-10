#include "Player/Behaviours/PlayerCharacterClassBase.h"

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerCharacterControllerBase.h"

void FPlayerCharacterClassBase::Initialize(const FCharacterClassInitializationInfo& info)
{
	_controller = info.Controller;
	_character = _controller->GetPlayerCharacter();
	_animInstance = Cast<UPlayerCharacterAnimInstanceBase>(_character->GetMesh()->GetAnimInstance());
	_movementSpeed = info.MovementSpeed;
}

void FPlayerCharacterClassBase::Tick(float deltaSeconds)
{
	if (!_isMovingEnabled)
	{
		_animInstance->SetMovementSpeed(0.0f);
		
		return;
	}
	
	const FVector directionVector = FVector(_directionVector, 0.0f);
	
	if (_directionVector != FVector2D::ZeroVector)
	{
		_controller->SetControlRotation(directionVector.Rotation());
	}
	
	_character->AddMovementInput(directionVector.GetClampedToMaxSize(1), _movementSpeed);

	_animInstance->SetMovementSpeed(directionVector.Length());
}

bool FPlayerCharacterClassBase::SetMovementDirection(const FVector2D& directionVector)
{
	if (_directionVector == directionVector) return false;
	
	_directionVector = directionVector;

	return true;
}

FVector2D FPlayerCharacterClassBase::GetMovementDirection() const
{
	return _directionVector;
}

