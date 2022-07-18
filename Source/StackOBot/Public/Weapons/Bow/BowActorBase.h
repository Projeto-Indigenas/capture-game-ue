#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponActorBase.h"
#include "BowActorBase.generated.h"

class AArrowActorBase;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API ABowActorBase : public AWeaponActorBase
{
	GENERATED_BODY()

	float _lastArrowFlyTime = 0.0f;

	bool CanFlyArrow(const float now) const;
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AArrowActorBase> _arrowClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _maxArrowFlyVelocity = 1000.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _arrowReleaseRateInSeconds = 1.0f;

public:
	bool CanFlyArrow() const;
	void FlyArrowFly(const FVector& direction, const float impulseMultiplier);
};
