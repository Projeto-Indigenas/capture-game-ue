#pragma once

#include "CoreMinimal.h"
#include "Weapons/DamageWeaponActorBase.h"
#include "ArrowActorBase.generated.h"

class UPhysicsSettings;
class UBoxComponent;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AArrowActorBase : public ADamageWeaponActorBase
{
	GENERATED_BODY()

	FVector _velocity = FVector::ZeroVector;
	bool _isFlying = false;

	UPROPERTY()
	const UPhysicsSettings* _physicsSettings;

	void DisableAndScheduleDestroy();
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBoxComponent* _boxComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _delayToDestroy = 5.0f;

	virtual void OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent) override;
	
	virtual void BeginPlay() override;

public:
	AArrowActorBase();

	virtual void Tick(float deltaSeconds) override;
	
	void BeginFlying(AActor* ownerActor, const FVector& velocity);
};
