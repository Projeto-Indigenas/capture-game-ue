#include "Player/Class/PlayerCharacterClassBase.h"

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerCharacterControllerBase.h"

void UPlayerCharacterClassBase::Initialize(const FCharacterClassInitializationInfo& info)
{
	_controller = info.Controller;
	_character = info.Character;
	_animInstance = Cast<UPlayerCharacterAnimInstanceBase>(_character->GetMesh()->GetAnimInstance());
	_movementSpeed = info.MovementSpeed;
	_movementSpeedDebuff = info.MovementSpeedDebuff;

	_animInstance->SetFalling(_character->bClientWasFalling);
}

void UPlayerCharacterClassBase::Tick(float deltaSeconds)
{
	const float movementSpeedDebuff = _shouldDebuffMovement ? _movementSpeedDebuff : 1.0f;
	
	const FVector directionVector = FVector(_directionVector, 0.0f).GetClampedToMaxSize(1);

	if (_controller.IsValid() && _directionVector != FVector2D::ZeroVector &&
		!_animInstance->IsFalling())
	{
		_controller->SetControlRotation(directionVector.Rotation());
	}
	
	_character->AddMovementInput(directionVector, _movementSpeed * movementSpeedDebuff);

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

bool UPlayerCharacterClassBase::PrimaryAttack()
{
	if (_animInstance->IsCarryingItem()) return false;
	return _animInstance->PrimaryAttack();
}

bool UPlayerCharacterClassBase::EvadeAttack()
{
	if (_animInstance->IsCarryingItem()) return false;
	return _animInstance->EvadeAttack();
}

bool UPlayerCharacterClassBase::TakeHit()
{
	return _animInstance->TakeHit();
}

void UPlayerCharacterClassBase::SetCarryingItem(const bool carrying)
{
	_shouldDebuffMovement = carrying;
	_animInstance->SetCarryingItem(carrying);
}

