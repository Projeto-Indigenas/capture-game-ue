#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PlayerCharacter, Log, All);

class UWeaponComponent;
class UPlayerCharacterAnimInstanceBase;
class APlayerCharacterControllerBase;
class APlayerCharacterRagdollBase;
class UPlayerCharacterClassBase;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY()
	UPlayerCharacterClassBase* _playerCharacterClass;
	
	TWeakObjectPtr<UWeaponComponent> _weaponComponent;
	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;

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

	void SetAnimMovementSpeed(float speed) const;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateTakeDamage_Client(float damage);

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _movementSpeed = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _initialLifePoints = 100.0f;
	
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
};
