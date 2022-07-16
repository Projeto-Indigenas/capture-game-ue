#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponActorBase.h"
#include "BowActorBase.generated.h"

class AArrowActorBase;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API ABowActorBase : public AWeaponActorBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AArrowActorBase> _arrowClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _maxArrowFlyVelocity = 1000.0f;

public:
	void FlyArrowFly(const FVector& direction, const float impulseMultiplier) const;
};
