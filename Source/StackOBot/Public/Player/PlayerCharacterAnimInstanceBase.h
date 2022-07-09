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
	
	UPROPERTY(BlueprintReadWrite)
	float _movementSpeed;

	UPROPERTY(BlueprintReadWrite)
	bool _primaryAttack;

	UPROPERTY(BlueprintReadWrite)
	bool _evadeAttack;

	UPROPERTY(BlueprintReadWrite)
	bool _takeHit;

	UFUNCTION(BlueprintCallable)
	void NotifyPrimaryAttackFinished();

	UFUNCTION(BlueprintCallable)
	void NotifyEvadeAttackFinished();
	
public:
	TFunction<void()> PrimaryAttackFinished;
	TFunction<void()> EvadeAttackFinished;
	
	bool SetMovementSpeed(float speed);
	bool PrimaryAttack();
	bool EvadeAttack();
	bool TakeHit();

	float GetMovementSpeed() const;
	bool IsAttacking() const;
	bool IsEvading() const;
	bool IsTakingHit() const;
};
