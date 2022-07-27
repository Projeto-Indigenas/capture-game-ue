#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterClassBase.h"
#include "Misc/NetHelpers.h"
#include "Misc/Vectors.h"
#include "ArcherPlayerCharacterClass.generated.h"

class APlayerCameraActor;
class ABowActorBase;

struct STACKOBOT_API FArcherCharacterClassInitializationInfo : FCharacterClassInitializationInfo
{
	const TWeakObjectPtr<ABowActorBase>& BowWeapon;
	const TLocalRoleHelper<APlayerCharacterBase, void>& LetTheArrowFlyFunction;

	FArcherCharacterClassInitializationInfo(
		const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
		const TWeakObjectPtr<APlayerCharacterBase>& character,
		const float movementSpeed,
		const float movementSpeedDebuff,
		const float lookToDirectionAcceleration,
		const FName& resourceItemSocketName,
		const TWeakObjectPtr<ABowActorBase>& bowWeapon,
		const TLocalRoleHelper<APlayerCharacterBase, void>& letTheArrowFlyFunction);
};

UCLASS()
class STACKOBOT_API UArcherPlayerCharacterClass : public UPlayerCharacterClassBase
{
	GENERATED_BODY()
	
	FAcceleratedVector2D _lastKnownDirection;
	bool _finishedReleasingArrow = true;
	
	TWeakObjectPtr<APlayerCameraActor> _cameraActor;
	TWeakObjectPtr<ABowActorBase> _bowWeapon;
	
	TLocalRoleHelper<APlayerCharacterBase, void> _letTheArrowFlyFunction;
	TLocalRoleHelper<UArcherPlayerCharacterClass, bool, const bool> _primaryAttackSwitcher;

	virtual void Initialize(const FCharacterClassInitializationInfo& info) override;

	void ReleaseArrowFinished();

protected:
	virtual FVector2D GetMovementDirection(const FVector2D& direction) override;
	virtual void UpdateCharacterRotation(const FVector& direction) override;
	
public:
	virtual void Initialize(const FArcherCharacterClassInitializationInfo& info);
	virtual void DeInitialize() override;

	virtual void Tick(float deltaSeconds) override;

	virtual void SetAimDirection(const FVector2D& directionVector) override;
	virtual FVector2D GetAimDirection() const override;
	virtual void LetTheArrowFly() override;

	virtual bool PrimaryAttack(const bool pressed) override;
	
	virtual ECharacterClassType GetClassType() const override;
};
