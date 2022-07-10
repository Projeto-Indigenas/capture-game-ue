#pragma once

#include "CoreMinimal.h"

class APlayerCharacterControllerBase;
class APlayerCharacterBase;
class UPlayerCharacterAnimInstanceBase;

struct STACKOBOT_API FCharacterClassInitializationInfo
{
	APlayerCharacterControllerBase* Controller = nullptr;
	float MovementSpeed = 0.0f;
};

class STACKOBOT_API FPlayerCharacterClassBase
{
	TWeakObjectPtr<APlayerCharacterControllerBase> _controller;
	TWeakObjectPtr<APlayerCharacterBase> _character;
	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;

	FVector2D _directionVector = FVector2D::ZeroVector;
	float _movementSpeed = 0.0f;

protected:
	bool _isMovingEnabled = true;
	
public:
	virtual ~FPlayerCharacterClassBase() = default;

	virtual void Initialize(const FCharacterClassInitializationInfo& info);
	virtual void Tick(float deltaSeconds);

	virtual bool SetMovementDirection(const FVector2D& directionVector);
	virtual FVector2D GetMovementDirection() const;
};
