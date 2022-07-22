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

	FTimerHandle _destroyTimerHandle;
	
	UPROPERTY(Replicated)
	FVector _velocity = FVector::ZeroVector;
	
	UPROPERTY(Replicated)
	bool _isFlying = false;

	void DisableAndScheduleDestroy();

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateDisable_Clients(UPrimitiveComponent* otherComponent);
	
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
