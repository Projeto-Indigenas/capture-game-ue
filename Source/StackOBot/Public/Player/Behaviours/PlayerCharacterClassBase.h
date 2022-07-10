#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterClassBase.generated.h"

class APlayerCharacterControllerBase;
class APlayerCharacterBase;
class UPlayerCharacterAnimInstanceBase;

struct STACKOBOT_API FCharacterClassInitializationInfo
{
	APlayerCharacterControllerBase* Controller = nullptr;
	APlayerCharacterBase* Character = nullptr;
	float MovementSpeed = 0.0f;
};

UCLASS()
class STACKOBOT_API UPlayerCharacterClassBase : public UObject
{
	GENERATED_BODY()
	
	TWeakObjectPtr<APlayerCharacterControllerBase> _controller;
	TWeakObjectPtr<APlayerCharacterBase> _character;
	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;

	FVector2D _directionVector = FVector2D::ZeroVector;
	float _movementSpeed = 0.0f;

protected:
	bool _isMovingEnabled = true;
	
public:
	virtual void Initialize(const FCharacterClassInitializationInfo& info);
	virtual void Tick(float deltaSeconds);

	virtual void OnFalling();
	virtual void OnLanded();

	virtual bool SetMovementDirection(const FVector2D& directionVector);
	virtual FVector2D GetMovementDirection() const;
	
	virtual bool Jump();
};
