#include "Player/PlayerCharacterAnimInstanceBase.h"

void UPlayerCharacterAnimInstanceBase::NotifyPrimaryAttackFinished()
{
	_primaryAttack = false;
	
	if (!PrimaryAttackFinished) return;
	
	PrimaryAttackFinished();
}

void UPlayerCharacterAnimInstanceBase::NotifyEvadeAttackFinished()
{
	_evadeAttack = false;
	
	if (!EvadeAttackFinished) return;

	EvadeAttackFinished();
}

void UPlayerCharacterAnimInstanceBase::NotifyTakeHitFinished()
{
	_takeHit = false;

	if (!TakeHitFinished) return;

	TakeHitFinished();
}

void UPlayerCharacterAnimInstanceBase::NotifyPrimaryAttackHitEnabled(const bool enabled)
{
	if (!PrimaryAttackHitEnabled) return;

	PrimaryAttackHitEnabled(enabled);
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
	
	_primaryAttack = false;
	return _evadeAttack = true;
}

bool UPlayerCharacterAnimInstanceBase::TakeHit()
{
	if (_takeHit) return false;

	_primaryAttack = false;
	_evadeAttack = false;
	return _takeHit = true;
}

void UPlayerCharacterAnimInstanceBase::SetJumping(const bool jumping)
{
	_jump = jumping;
}

void UPlayerCharacterAnimInstanceBase::SetFalling(const bool falling)
{
	_falling = falling;
}

void UPlayerCharacterAnimInstanceBase::SetCarryingItem(const bool carrying)
{
	_carryingItem = carrying;
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

bool UPlayerCharacterAnimInstanceBase::IsCarryingItem() const
{
	return _carryingItem;
}

bool UPlayerCharacterAnimInstanceBase::IsFalling() const
{
	return _jump || _falling;
}
