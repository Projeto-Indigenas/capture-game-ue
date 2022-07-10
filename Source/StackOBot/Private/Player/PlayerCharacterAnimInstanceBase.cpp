#include "Player/PlayerCharacterAnimInstanceBase.h"

void UPlayerCharacterAnimInstanceBase::NotifyPrimaryAttackFinished()
{
	if (!PrimaryAttackFinished) return;
	
	PrimaryAttackFinished();
}

void UPlayerCharacterAnimInstanceBase::NotifyEvadeAttackFinished()
{
	if (!EvadeAttackFinished) return;

	EvadeAttackFinished();
}

bool UPlayerCharacterAnimInstanceBase::SetMovementSpeed(float speed)
{
	const float newSpeed = speed * _movementSpeedScale;

	if (newSpeed != _movementSpeed)
	{
		_movementSpeed = newSpeed;

		return true;
	}

	return false;
}

bool UPlayerCharacterAnimInstanceBase::PrimaryAttack()
{
	if (_primaryAttack) return false;
	
	return _primaryAttack = true;
}

bool UPlayerCharacterAnimInstanceBase::EvadeAttack()
{
	if (_evadeAttack) return false;
	
	return _evadeAttack = true;
}

bool UPlayerCharacterAnimInstanceBase::TakeHit()
{
	if (_takeHit) return false;
	
	return _takeHit = true;
}

void UPlayerCharacterAnimInstanceBase::SetJumping(const bool jumping)
{
	_jump = jumping;
}

void UPlayerCharacterAnimInstanceBase::SetFalling(bool falling)
{
	_falling = falling;
}

float UPlayerCharacterAnimInstanceBase::GetMovementSpeed() const
{
	return _movementSpeed;
}

bool UPlayerCharacterAnimInstanceBase::IsAttacking() const
{
	return _primaryAttack;
}

bool UPlayerCharacterAnimInstanceBase::IsEvading() const
{
	return _evadeAttack;
}

bool UPlayerCharacterAnimInstanceBase::IsTakingHit() const
{
	return _takeHit;
}

bool UPlayerCharacterAnimInstanceBase::IsFalling() const
{
	return _jump || _falling;
}
