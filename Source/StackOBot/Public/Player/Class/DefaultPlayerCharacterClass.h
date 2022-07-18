#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterClassBase.h"
#include "DefaultPlayerCharacterClass.generated.h"

class ADamageWeaponActorBase;
class UBoxComponent;

struct STACKOBOT_API FDefaultCharacterClassInitializationInfo : FCharacterClassInitializationInfo
{
	ADamageWeaponActorBase* Weapon = nullptr;
	UBoxComponent* HitBox = nullptr;

	FDefaultCharacterClassInitializationInfo(
		const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
		const TWeakObjectPtr<APlayerCharacterBase>& character,
		const float movementSpeed,
		const float movementSpeedDebuff,
		const float lookToDirectionAcceleration,
		const FName& resourceItemSocketName,
		ADamageWeaponActorBase* weapon,
		UBoxComponent* hitBox);
};

UCLASS()
class STACKOBOT_API UDefaultPlayerCharacterClass : public UPlayerCharacterClassBase
{
	GENERATED_BODY()

	TWeakObjectPtr<ADamageWeaponActorBase> _theStickWeapon;
	TWeakObjectPtr<UBoxComponent> _hitBox;

	void ChangeWeaponHitEnabled(bool hitEnabled) const;

	// just to hide the method
	virtual void Initialize(const FCharacterClassInitializationInfo& info) override;
	
public:
	virtual void Initialize(const FDefaultCharacterClassInitializationInfo& info);
	virtual void DeInitialize() override;

	virtual bool PrimaryAttack(const bool pressed) override;
	virtual bool TakeHit() override;
};
