#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterAnimInstanceBase.generated.h"

enum class ECharacterClassType : uint8;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API UPlayerCharacterAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _movementSpeedScale = 100.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ECharacterClassType _classType;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector2D _movementDirection;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool _primaryAttack;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool _takeHit;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool _jump;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool _falling;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool _carryingItem;

	UFUNCTION(BlueprintCallable)
	void NotifyPrimaryAttackFinished();

	UFUNCTION(BlueprintCallable)
	void NotifyTakeHitFinished();

	UFUNCTION(BlueprintCallable)
	void NotifyPrimaryAttackHitEnabled(const bool enabled);

	UFUNCTION(BlueprintCallable)
	void NotifyReleaseArrow();

	UFUNCTION(BlueprintCallable)
	void NotifyReleaseArrowFinished();

public:
	TFunction<void()> PrimaryAttackFinished;
	TFunction<void(bool hitEnabled)> PrimaryAttackHitEnabled;
	TFunction<void()> TakeHitFinished;

	TFunction<void()> ReleaseArrow;
	TFunction<void()> ReleaseArrowFinished;

	void SetCharacterClass(const ECharacterClassType classType);
	
	bool SetMovementDirection(const FVector2D& direction);
	bool SetPrimaryAttack(const bool attack);
	bool TakeHit();
	
	void SetJumping(const bool jumping);
	void SetFalling(const bool falling);
	void SetCarryingItem(const bool carrying);

	float GetMovementSpeed() const;
	bool IsAttacking() const;
	bool IsTakingHit() const;
	bool IsCarryingItem() const;

	bool IsFalling() const;
};
