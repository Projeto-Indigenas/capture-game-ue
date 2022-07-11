#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterAnimInstanceBase.generated.h"

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API UPlayerCharacterAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _movementSpeedScale = 100.0f;
	
	UPROPERTY(BlueprintReadOnly)
	float _movementSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool _primaryAttack;

	UPROPERTY(BlueprintReadOnly)
	bool _evadeAttack;

	UPROPERTY(BlueprintReadOnly)
	bool _takeHit;

	UPROPERTY(BlueprintReadOnly)
	bool _jump;

	UPROPERTY(BlueprintReadOnly)
	bool _falling;

	UPROPERTY(BlueprintReadOnly)
	bool _carryingItem;

	UFUNCTION(BlueprintCallable)
	void NotifyPrimaryAttackFinished();

	UFUNCTION(BlueprintCallable)
	void NotifyEvadeAttackFinished();

	UFUNCTION(BlueprintCallable)
	void NotifyTakeHitFinished();

	UFUNCTION(BlueprintCallable)
	void NotifyPrimaryAttackHitEnabled(const bool enabled);
	
public:
	TFunction<void()> PrimaryAttackFinished;
	TFunction<void(bool hitEnabled)> PrimaryAttackHitEnabled;
	TFunction<void()> EvadeAttackFinished;
	TFunction<void()> TakeHitFinished;
	
	bool SetMovementSpeed(float speed);
	bool PrimaryAttack();
	bool EvadeAttack();
	bool TakeHit();
	
	void SetJumping(const bool jumping);
	void SetFalling(const bool falling);
	void SetCarryingItem(const bool carrying);

	float GetMovementSpeed() const;
	bool IsAttacking() const;
	bool IsEvading() const;
	bool IsTakingHit() const;
	bool IsCarryingItem() const;

	bool IsFalling() const;
};
