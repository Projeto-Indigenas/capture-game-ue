#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PlayerCharacter, Log, All);

class AConstructionResourcePieceActorBase;
class AConstructionBuildingBase;
class APlayerCharacterControllerBase;
class APlayerCharacterRagdollBase;
class AWeaponActorBase;
class UPlayerCharacterClassBase;
class UPlayerCharacterAnimInstanceBase;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

	TWeakObjectPtr<AWeaponActorBase> _weaponActor;
	
	UPROPERTY()
	UPlayerCharacterClassBase* _playerCharacterClass;
	
	void LogOnScreen(const FString& message) const;

	UFUNCTION(Server, Unreliable)
	void ReplicateMovementDirection_Server(const FVector2D& directionVector);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateMovementDirection_Clients(const FVector2D& directionVector);

	UFUNCTION(Server, Unreliable)
	void ReplicateRequestJump_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateRequestJump_Clients();

	UFUNCTION(Server, Unreliable)
	void ReplicatePrimaryAttack_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicatePrimaryAttack_Client();

	UFUNCTION(Server, Unreliable)
	void ReplicateEvadeAttack_Server();
	
	UFUNCTION()
	void TakeAnyDamage(AActor* damagedActor, float damage, const UDamageType* damageType,
		AController* instigatedBy, AActor* damageCauser);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateTakeDamage_Clients(float damage);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateCharacterDeath_Clients();

	UFUNCTION(Server, Reliable)
	void ReplicatePickUpItem_Server();

	UFUNCTION(NetMulticast, Reliable)
	void ReplicateCarryItem_Clients(AConstructionResourcePieceActorBase* piece);

	UFUNCTION(NetMulticast, Reliable)
	void ReplicateDropItem_Clients(AConstructionResourcePieceActorBase* piece);
	
	void CharacterDied() const;
	void PickDropItem();

protected:
	UPROPERTY(EditDefaultsOnly)
	float _movementSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float _movementSpeedDebuff = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float _initialLifePoints = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	FName _resourceItemSocketName = NAME_None;
	
	UPROPERTY(Replicated)
	float _currentLifePoints = 100.0f;

	virtual void BeginPlay() override;

public:
	TDelegate<void()> OnCharacterDeath;

	virtual void Initialize(APlayerCharacterControllerBase* controller);
	
	virtual void Tick(float deltaSeconds) override;
	virtual void Falling() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	
	void SetMovementDirection(const FVector2D& directionVector);
	void PrimaryAttack();
	void EvadeAttack();
	void RequestJump();
	void PickUpItem();
};
