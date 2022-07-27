#include "Player/Class/DefaultPlayerCharacterClass.h"

#include <Components/BoxComponent.h>
#include <Kismet/GameplayStatics.h>

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Weapons/DamageWeaponActorBase.h"

FDefaultCharacterClassInitializationInfo::FDefaultCharacterClassInitializationInfo(
const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
	const TWeakObjectPtr<APlayerCharacterBase>& character,
	const float movementSpeed,
	const float movementSpeedDebuff,
	const float lookToDirectionAcceleration,
	const FName& resourceItemSocketName,
	ADamageWeaponActorBase* weapon,
	UBoxComponent* hitBox) : FCharacterClassInitializationInfo(
		controller, character, movementSpeed, movementSpeedDebuff,
		lookToDirectionAcceleration, resourceItemSocketName),
	Weapon(weapon),
	HitBox(hitBox)
{
	//
}

void UDefaultPlayerCharacterClass::ChangeWeaponHitEnabled(bool hitEnabled) const
{
	_hitBox->SetGenerateOverlapEvents(hitEnabled);
	_hitBox->UpdateOverlaps();
}

void UDefaultPlayerCharacterClass::Initialize(const FCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);
}

void UDefaultPlayerCharacterClass::Initialize(const FDefaultCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);

	_theStickWeapon = info.Weapon;
	_hitBox = info.HitBox;
	
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
		_hitBox->SetGenerateOverlapEvents(false);
	};

	_theStickWeapon->SetActorHiddenInGame(false);

	if (_character->HasAuthority())
	{
		_hitBox->OnComponentBeginOverlap.AddDynamic(_theStickWeapon.Get(),
			&ADamageWeaponActorBase::OnBeginOverlap);
	}
}

void UDefaultPlayerCharacterClass::DeInitialize()
{
	Super::DeInitialize();

	_theStickWeapon->SetActorHiddenInGame(true);
}

bool UDefaultPlayerCharacterClass::PrimaryAttack(const bool pressed)
{
	if (!pressed) return false;
	if (_animInstance->IsAttacking()) return false;

	if (Super::PrimaryAttack(pressed))
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
		_hitBox->SetGenerateOverlapEvents(false);

		return true;
	}

	return false;
}
