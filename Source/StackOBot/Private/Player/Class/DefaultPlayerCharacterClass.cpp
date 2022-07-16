#include "Player/Class/DefaultPlayerCharacterClass.h"

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Weapons/DamageWeaponActorBase.h"
#include "Weapons/WeaponActorBase.h"

FDefaultCharacterClassInitializationInfo::FDefaultCharacterClassInitializationInfo(
const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
	const TWeakObjectPtr<APlayerCharacterBase>& character,
	const float movementSpeed,
	const float movementSpeedDebuff,
	const float lookToDirectionAcceleration,
	const FName& resourceItemSocketName,
	const TWeakObjectPtr<ADamageWeaponActorBase>& weapon) : FCharacterClassInitializationInfo(
		controller, character, movementSpeed, movementSpeedDebuff,
		lookToDirectionAcceleration, resourceItemSocketName),
	Weapon(weapon)
{
	//
}

void UDefaultPlayerCharacterClass::ChangeWeaponHitEnabled(bool hitEnabled) const
{
	if (!_theStickWeapon.IsValid()) return;

	_theStickWeapon->SetGenerateOverlapEvents(hitEnabled);
}

void UDefaultPlayerCharacterClass::Initialize(const FCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);
}

void UDefaultPlayerCharacterClass::Initialize(const FDefaultCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);

	_theStickWeapon = info.Weapon;
	
	if (_character->HasAuthority())
	{
		_animInstance->PrimaryAttackHitEnabled = [this](bool hitEnabled)
		{
			ChangeWeaponHitEnabled(hitEnabled);
		};
	}

	_animInstance->PrimaryAttackFinished = [this]
	{
		_shouldDebuffMovement = false;
		_theStickWeapon->SetGenerateOverlapEvents(false);
	};

	_theStickWeapon->SetActorHiddenInGame(false);
}

void UDefaultPlayerCharacterClass::DeInitialize()
{
	Super::DeInitialize();

	_theStickWeapon->SetActorHiddenInGame(true);
}

bool UDefaultPlayerCharacterClass::PrimaryAttack(const bool pressed)
{
	if (!pressed) return false;
	if (_animInstance->IsCarryingItem()) return false;
	if (_animInstance->IsAttacking()) return false;

	_animInstance->SetPrimaryAttack(true);
	_shouldDebuffMovement = true;

	return true;
}

bool UDefaultPlayerCharacterClass::TakeHit()
{
	if (Super::TakeHit())
	{
		_shouldDebuffMovement = true;
		_theStickWeapon->SetGenerateOverlapEvents(false);

		return true;
	}

	return false;
}
