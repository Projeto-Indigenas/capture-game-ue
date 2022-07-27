#include "Player/Class/ArcherPlayerCharacterClass.h"

#include <stdbool.h>

#include "Kismet/GameplayStatics.h"

#include "Player/PlayerCameraActor.h"
#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerCharacterControllerBase.h"
#include "Player/Class/CharacterClassType.h"
#include "Weapons/Bow/BowActorBase.h"

FArcherCharacterClassInitializationInfo::FArcherCharacterClassInitializationInfo(
const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
	const TWeakObjectPtr<APlayerCharacterBase>& character,
	const float movementSpeed, const float movementSpeedDebuff,
	const float lookToDirectionAcceleration,
	const FName& resourceItemSocketName,
	const TWeakObjectPtr<ABowActorBase>& bowWeapon,
	const TLocalRoleHelper<APlayerCharacterBase, void>& letTheArrowFlyFunction) :
	FCharacterClassInitializationInfo(
		controller, character, movementSpeed, movementSpeedDebuff,
		lookToDirectionAcceleration, resourceItemSocketName),
	BowWeapon(bowWeapon),
	LetTheArrowFlyFunction(letTheArrowFlyFunction)
{
	//
}

void UArcherPlayerCharacterClass::Initialize(const FCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);
}

void UArcherPlayerCharacterClass::ReleaseArrowFinished()
{
	_finishedReleasingArrow = true;
}

FVector2D UArcherPlayerCharacterClass::GetMovementDirection(const FVector2D& direction)
{
	if (_animInstance->IsAttacking() || !_finishedReleasingArrow)
	{
		FVector inverseDirection(direction, 0.0f);
 		inverseDirection = _character->GetActorTransform().InverseTransformVector(inverseDirection);
		const FVector2D copyDirection(inverseDirection.Y, inverseDirection.X);
		return Super::GetMovementDirection(copyDirection);
	}

	const FVector2D copyDirection(0.0f, FMath::Min(direction.Length(), 1.0f));
	
	return Super::GetMovementDirection(copyDirection);
}

void UArcherPlayerCharacterClass::UpdateCharacterRotation(const FVector& direction)
{
	if (_animInstance->IsAttacking() || !_finishedReleasingArrow)
	{
		Super::UpdateCharacterRotation(_lastKnownDirection);
		
		return;
	}
	
	Super::UpdateCharacterRotation(direction);
}

void UArcherPlayerCharacterClass::Initialize(const FArcherCharacterClassInitializationInfo& info)
{
	Super::Initialize(info);

	_lastKnownDirection.SetAcceleration(5.0f);
	
	if (_controller.IsValid())
	{
		_cameraActor = _controller->GetCameraActor();
	}

	_animInstance->ReleaseArrow = [this] { LetTheArrowFly(); };
	_animInstance->ReleaseArrowFinished = [this] { ReleaseArrowFinished(); };

	_bowWeapon = info.BowWeapon;
	_letTheArrowFlyFunction = info.LetTheArrowFlyFunction;

	_bowWeapon->SetActorHiddenInGame(false);

	_primaryAttackSwitcher = TLocalRoleHelper<UArcherPlayerCharacterClass, bool, const bool>()
		.AutonomousProxy([this](const bool attack)
		{
			if (_animInstance->IsCarryingItem()) return false;

			return Super::PrimaryAttack(attack);
		})
		.Authority([this](const bool attack)
		{
			if (_animInstance->IsCarryingItem()) return false;
	
			if (!attack && !_bowWeapon->CanFlyArrow()) return false;

			if (Super::PrimaryAttack(attack))
			{
				if (attack)
				{
					_lastKnownDirection = _character->GetActorRotation().Vector();
				}
				else
				{
					_finishedReleasingArrow = false;
				}

				return true;
			}

			return false;
		});
}

void UArcherPlayerCharacterClass::DeInitialize()
{
	Super::DeInitialize();

	_bowWeapon->SetActorHiddenInGame(true);
}

void UArcherPlayerCharacterClass::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	_lastKnownDirection.Tick(deltaSeconds);
}

void UArcherPlayerCharacterClass::SetAimDirection(const FVector2D& directionVector)
{
	if (directionVector.IsZero()) return;
	if (_lastKnownDirection.GetTarget2D() == directionVector) return;

	_lastKnownDirection = directionVector;
}

FVector2D UArcherPlayerCharacterClass::GetAimDirection() const
{
	return _lastKnownDirection;
}

void UArcherPlayerCharacterClass::LetTheArrowFly()
{
	if (!_character->HasAuthority()) return;
	
	// TODO(anderson): the intensity should be passed instead of the 1.0f
	_bowWeapon->FlyArrowFly(_lastKnownDirection.GetTarget(), 1.0f);
	
	_finishedReleasingArrow = false;

	_letTheArrowFlyFunction.Switch(_character.Get());
}

bool UArcherPlayerCharacterClass::PrimaryAttack(const bool pressed)
{
	return _primaryAttackSwitcher.Switch(_character.Get(), pressed);
}

ECharacterClassType UArcherPlayerCharacterClass::GetClassType() const
{
	return ECharacterClassType::Archer;
}
