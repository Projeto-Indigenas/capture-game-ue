#include "Player/Class/DefaultPlayerCharacterClass.h"

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Weapons/WeaponActorBase.h"

void UDefaultPlayerCharacterClass::ChangeWeaponHitEnabled(bool hitEnabled) const
{
	if (!_weapon.IsValid()) return;

	_weapon->SetGenerateOverlapEvents(hitEnabled);
}

void UDefaultPlayerCharacterClass::Initialize(const FCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);
}

void UDefaultPlayerCharacterClass::Initialize(const FDefaultCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);

	_weapon = info.Weapon;
	
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
		_weapon->SetGenerateOverlapEvents(false);
	};

	_animInstance->TakeHitFinished = [this]
	{
		_shouldDebuffMovement = false;	
	};
}

bool UDefaultPlayerCharacterClass::PrimaryAttack()
{
	if (Super::PrimaryAttack())
	{
		_shouldDebuffMovement = true;
		
		return true;
	}
	
	return false;
}

bool UDefaultPlayerCharacterClass::TakeHit()
{
	if (Super::TakeHit())
	{
		_shouldDebuffMovement = true;
		_weapon->SetGenerateOverlapEvents(false);

		return true;
	}

	return false;
}
