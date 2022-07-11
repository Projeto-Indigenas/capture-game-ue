#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterClassBase.h"
#include "DefaultPlayerCharacterClass.generated.h"

class AWeaponActorBase;

struct STACKOBOT_API FDefaultCharacterClassInitializationInfo : FCharacterClassInitializationInfo
{
	AWeaponActorBase* Weapon;
};

UCLASS()
class STACKOBOT_API UDefaultPlayerCharacterClass : public UPlayerCharacterClassBase
{
	GENERATED_BODY()

	TWeakObjectPtr<AWeaponActorBase> _weapon;

	void ChangeWeaponHitEnabled(bool hitEnabled) const;

	// just to hide the method
	virtual void Initialize(const FCharacterClassInitializationInfo& info) override;
	
public:
	virtual void Initialize(const FDefaultCharacterClassInitializationInfo& info);

	virtual bool PrimaryAttack() override;
	virtual bool TakeHit() override;
};
