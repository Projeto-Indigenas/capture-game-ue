#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterClassBase.generated.h"

class AConstructionResourcePieceActorBase;
class APlayerCharacterControllerBase;
class APlayerCharacterBase;
class AConstructionBuildingBase;
class UPlayerCharacterAnimInstanceBase;

struct STACKOBOT_API FCharacterClassInitializationInfo
{
	APlayerCharacterControllerBase* Controller = nullptr;
	APlayerCharacterBase* Character = nullptr;
	float MovementSpeed = 1.0f;
	float MovementSpeedDebuff = 1.0f;
	FName ResourceItemSocketName = NAME_None;
};

UCLASS()
class STACKOBOT_API UPlayerCharacterClassBase : public UObject
{
	GENERATED_BODY()
	
	FVector2D _directionVector = FVector2D::ZeroVector;
	float _movementSpeed = 0.0f;
	float _movementSpeedDebuff = 1.0f;
	FName _resourceItemSocketName = NAME_None;
	
	TWeakObjectPtr<AConstructionResourcePieceActorBase> _resourcePieceAvailableToPick;
	TWeakObjectPtr<AConstructionResourcePieceActorBase> _carryingPiece;
	TWeakObjectPtr<AConstructionBuildingBase> _constructionBuilding;

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex,
		bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	virtual void SetCarryingItem(const bool carrying);

protected:
	TWeakObjectPtr<APlayerCharacterControllerBase> _controller;
	TWeakObjectPtr<APlayerCharacterBase> _character;
	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;
	TWeakObjectPtr<USkeletalMeshComponent> _skeletalMesh;
	
	bool _shouldDebuffMovement = false;
	
public:
	virtual void Initialize(const FCharacterClassInitializationInfo& info);
	virtual void Tick(float deltaSeconds);

	virtual void OnFalling();
	virtual void OnLanded();

	virtual bool SetMovementDirection(const FVector2D& directionVector);
	virtual FVector2D GetMovementDirection() const;
	
	virtual bool Jump();
	virtual bool PrimaryAttack();
	virtual bool EvadeAttack();
	virtual bool TakeHit();

	void PickDropItem(
		const TUniqueFunction<void(AConstructionResourcePieceActorBase*)>& itemPicked,
		const TUniqueFunction<void(AConstructionResourcePieceActorBase*)>& itemDropped);
	void CarryItem(AConstructionResourcePieceActorBase* piece);
	void DropItem(AConstructionResourcePieceActorBase* piece);
};
