#pragma once

#include "CoreMinimal.h"
#include "Misc/Vectors.h"
#include "PlayerCharacterClassBase.generated.h"

class AConstructionResourcePieceActorBase;
class APlayerCharacterControllerBase;
class APlayerCharacterBase;
class AConstructionBuildingBase;
class UPlayerCharacterAnimInstanceBase;
enum class ECharacterClassType : uint8;

struct STACKOBOT_API FCharacterClassInitializationInfo
{
	const TWeakObjectPtr<APlayerCharacterControllerBase>& Controller = nullptr;
	const TWeakObjectPtr<APlayerCharacterBase>& Character = nullptr;
	float MovementSpeed = 1.0f;
	float MovementSpeedDebuff = 1.0f;
	float LookToDirectionAcceleration = 1.0f;
	const FName& ResourceItemSocketName = NAME_None;

	FCharacterClassInitializationInfo(
		const TWeakObjectPtr<APlayerCharacterControllerBase>& controller,
		const TWeakObjectPtr<APlayerCharacterBase>& character,
		const float movementSpeed,
		const float movementSpeedDebuff,
		const float lookToDirectionAcceleration,
		const FName& resourceItemSocketName);
};

UCLASS()
class STACKOBOT_API UPlayerCharacterClassBase : public UObject
{
	GENERATED_BODY()
	
	FAcceleratedVector2D _directionVector = FAcceleratedVector2D::ZeroVector2D;
	float _movementSpeed = 0.0f;
	float _movementSpeedDebuff = 1.0f;
	FName _resourceItemSocketName = NAME_None;
	
	TWeakObjectPtr<AConstructionResourcePieceActorBase> _resourcePieceAvailableToPick;
	TWeakObjectPtr<AConstructionResourcePieceActorBase> _carryingPiece;
	TWeakObjectPtr<AConstructionBuildingBase> _constructionBuilding;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex,
		bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	void SetCarryingItem(const bool carrying);

protected:
	TWeakObjectPtr<APlayerCharacterControllerBase> _controller;
	TWeakObjectPtr<APlayerCharacterBase> _character;
	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;
	TWeakObjectPtr<USkeletalMeshComponent> _skeletalMesh;
	
	bool _shouldDebuffMovement = false;

	virtual FVector2D GetMovementDirection(const FVector2D& direction);
	virtual void UpdateCharacterRotation(const FVector& direction);
	
public:
	virtual void Initialize(const FCharacterClassInitializationInfo& info);
	virtual void DeInitialize();
	virtual void Tick(float deltaSeconds);

	virtual ECharacterClassType GetClassType() const;

	virtual void OnFalling();
	virtual void OnLanded();

	virtual bool SetMovementDirection(const FVector2D& directionVector);
	virtual FVector2D GetMovementDirection() const;

	virtual void SetAimDirection(const FVector2D& directionVector);
	virtual FVector2D GetAimDirection() const;
	
	virtual void Jump();
	virtual bool PrimaryAttack(const bool pressed, const bool isReplicated);
	virtual bool TakeHit();

	void PickDropItem(
		const TUniqueFunction<void(AConstructionResourcePieceActorBase*)>& itemPicked,
		const TUniqueFunction<void(AConstructionResourcePieceActorBase*)>& itemDropped);
	void CarryItem(AConstructionResourcePieceActorBase* piece);
	void DropItem(AConstructionResourcePieceActorBase* piece);
};
