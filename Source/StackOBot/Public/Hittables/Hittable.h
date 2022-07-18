#pragma once

#include "CoreMinimal.h"
#include "Hittable.generated.h"

class AActor;

UINTERFACE()
class STACKOBOT_API UHittable : public UInterface
{
	GENERATED_BODY()
};

class STACKOBOT_API IHittable
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual bool IsHittableByActor(AActor* damageCauser);
	
	UFUNCTION()
	virtual void TakeHit(AActor* damageCauser, const float damage);
};
