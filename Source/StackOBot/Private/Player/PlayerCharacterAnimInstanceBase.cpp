#include "PlayerCharacterAnimInstanceBase.h"

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

void UPlayerCharacterAnimInstanceBase::SetLeanHorizontal(float horizontal)
{
	_leanHorizontal = horizontal;
}
