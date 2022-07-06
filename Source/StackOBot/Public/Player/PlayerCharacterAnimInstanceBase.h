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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float _movementSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float _leanHorizontal;
	
public:
	bool SetMovementSpeed(float speed);
	void SetLeanHorizontal(float horizontal);
};
