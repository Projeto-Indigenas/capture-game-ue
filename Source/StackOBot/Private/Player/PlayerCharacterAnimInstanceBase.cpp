#include "Player/PlayerCharacterAnimInstanceBase.h"

void UPlayerCharacterAnimInstanceBase::NotifyPrimaryAttackFinished()
{
	_primaryAttack = false;
	
	if (!PrimaryAttackFinished) return;
	
	PrimaryAttackFinished();
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

void UPlayerCharacterAnimInstanceBase::NotifyReleaseArrow()
{
	if (!ReleaseArrow) return;

	ReleaseArrow();
}

void UPlayerCharacterAnimInstanceBase::NotifyReleaseArrowFinished()
{
	if (!ReleaseArrowFinished) return;

	ReleaseArrowFinished();
}

void UPlayerCharacterAnimInstanceBase::SetCharacterClass(const ECharacterClassType classType)
{
	_classType = classType;
}

bool UPlayerCharacterAnimInstanceBase::SetMovementDirection(const FVector2D& direction)
{
	const FVector2D newSpeed = direction * _movementSpeedScale;

	if (newSpeed != _movementDirection)
	{
		_movementDirection = newSpeed;

		return true;
	}

	return false;
}

bool UPlayerCharacterAnimInstanceBase::SetPrimaryAttack(const bool attack)
{
	if (_primaryAttack != attack)
	{
		_primaryAttack = attack;
		
		return true;
	}

	return false;
}

bool UPlayerCharacterAnimInstanceBase::TakeHit()
{
	if (_takeHit) return false;

	_primaryAttack = false;
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
	return _movementDirection.Y;
}

bool UPlayerCharacterAnimInstanceBase::IsAttacking() const
{
	return _primaryAttack;
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
