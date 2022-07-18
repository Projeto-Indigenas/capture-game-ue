#pragma once

#include "CoreMinimal.h"
#include "WeaponActorBase.h"
#include "DamageWeaponActorBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(DamageWeaponActorBase, Log, All);

class UBoxComponent;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API ADamageWeaponActorBase : public AWeaponActorBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	float _damage = 5.0f;
	
	virtual void LogOnScreen(const FString& message) const override;
	
	virtual void OnDamageOtherActor(AActor* damagedActor, UPrimitiveComponent* otherComponent);
	virtual void OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent);

public:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex,
		bool bFromSweep, const FHitResult& sweepResult);
};
