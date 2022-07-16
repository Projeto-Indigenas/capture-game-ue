#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterClassBase.h"
#include "DefaultPlayerCharacterClass.generated.h"

class ADamageWeaponActorBase;

struct STACKOBOT_API FDefaultCharacterClassInitializationInfo : FCharacterClassInitializationInfo
{
	const TWeakObjectPtr<ADamageWeaponActorBase>& Weapon = nullptr;

	FDefaultCharacterClassInitializationInfo(
		const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
		const TWeakObjectPtr<APlayerCharacterBase>& character,
		const float movementSpeed,
		const float movementSpeedDebuff,
		const float lookToDirectionAcceleration,
		const FName& resourceItemSocketName,
		const TWeakObjectPtr<ADamageWeaponActorBase>& weapon);
};

UCLASS()
class STACKOBOT_API UDefaultPlayerCharacterClass : public UPlayerCharacterClassBase
{
	GENERATED_BODY()

	TWeakObjectPtr<ADamageWeaponActorBase> _theStickWeapon;

	void ChangeWeaponHitEnabled(bool hitEnabled) const;

	// just to hide the method
	virtual void Initialize(const FCharacterClassInitializationInfo& info) override;
	
public:
	virtual void Initialize(const FDefaultCharacterClassInitializationInfo& info);
	virtual void DeInitialize() override;

	virtual bool PrimaryAttack(const bool pressed) override;
	virtual bool TakeHit() override;
};
