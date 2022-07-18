#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hittables/Hittable.h"
#include "PlayerCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PlayerCharacter, Log, All);

class AConstructionResourcePieceActorBase;
class AConstructionBuildingBase;
class APlayerCharacterControllerBase;
class APlayerCharacterRagdollBase;
class ABowActorBase;
class ADamageWeaponActorBase;
class APlayerCameraActor;
class UPlayerCharacterClassBase;
class UPlayerCharacterAnimInstanceBase;
class UBoxComponent;
enum class ECharacterClassType : uint8;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API APlayerCharacterBase : public ACharacter, public IHittable
{
	GENERATED_BODY()

	TWeakObjectPtr<APlayerCharacterControllerBase> _playerController;
	
	UPROPERTY()
	UPlayerCharacterClassBase* _playerCharacterClass;
	
	void LogOnScreen(const FString& message) const;

	UFUNCTION(Server, Unreliable)
	void ReplicateMovementDirection_Server(const FVector2D& directionVector);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateMovementDirection_Clients(const FVector2D& directionVector);
	
	UFUNCTION(Server, Unreliable)
	void ReplicateAimDirection_Server(const FVector2D& directionVector);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateAimDirection_Clients(const FVector2D& directionVector);

	UFUNCTION(Server, Unreliable)
	void ReplicateRequestJump_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicateRequestJump_Clients();

	UFUNCTION(Server, Unreliable)
	void ReplicatePrimaryAttack_Server(const bool pressed);

	UFUNCTION(NetMulticast, Unreliable)
	void ReplicatePrimaryAttack_Clients(const bool pressed);

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

	void CreateOrUpdateCharacterClass();

protected:
	UPROPERTY(EditDefaultsOnly)
	ECharacterClassType _classType;

	UPROPERTY(EditDefaultsOnly)
	float _movementSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float _movementSpeedDebuff = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float _lookToDirectionAcceleration = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float _initialLifePoints = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	FName _resourceItemSocketName = NAME_None;
	
	UPROPERTY(Replicated)
	float _currentLifePoints = 100.0f;

	UFUNCTION(BlueprintNativeEvent)
	ADamageWeaponActorBase* GetTheStick();

	UFUNCTION(BlueprintNativeEvent)
	ABowActorBase* GetTheBow();

	UFUNCTION(BlueprintNativeEvent)
	UBoxComponent* GetHitBox();

	virtual void BeginPlay() override;

public:
	TDelegate<void()> OnCharacterDeath;

	virtual void Initialize(
		APlayerCharacterControllerBase* controller);
	
	virtual void Tick(float deltaSeconds) override;
	virtual void Falling() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	UFUNCTION()
	virtual void TakeHit(AActor* damageCauser, const float damage) override;
	
	void SetMovementDirection(const FVector2D& directionVector);
	void SetAimDirection(const FVector2D& directionVector);
	void PrimaryAttack(const bool pressed);
	void RequestJump();
	void PickUpItem();
};
