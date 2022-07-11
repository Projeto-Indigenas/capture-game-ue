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
	float MovementSpeed = 1.0f;
	float MovementSpeedDebuff = 1.0f;
};

UCLASS()
class STACKOBOT_API UPlayerCharacterClassBase : public UObject
{
	GENERATED_BODY()
	
	FVector2D _directionVector = FVector2D::ZeroVector;
	float _movementSpeed = 0.0f;
	float _movementSpeedDebuff = 1.0f;

protected:
	TWeakObjectPtr<APlayerCharacterControllerBase> _controller;
	TWeakObjectPtr<APlayerCharacterBase> _character;
	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;
	
	bool _shouldDebuffMovement = false;
	
public:
	virtual void Initialize(const FCharacterClassInitializationInfo& info);
	virtual void Tick(float deltaSeconds);

	virtual void OnFalling();
	virtual void OnLanded();

	virtual bool SetMovementDirection(const FVector2D& directionVector);
	virtual FVector2D GetMovementDirection() const;
	
	virtual bool Jump();
	virtual bool PrimaryAttack();
	virtual bool EvadeAttack();
	virtual bool TakeHit();

	virtual void SetCarryingItem(const bool carrying);
};
