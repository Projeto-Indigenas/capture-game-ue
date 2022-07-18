#pragma once

#include "CoreMinimal.h"
#include "Hittables/Hittable.h"
#include "ConstructionResourceActorBase.generated.h"

class AConstructionResourcePieceActorBase;
enum class EConstructionResourceType : uint8;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AConstructionResourceActorBase : public AActor, public IHittable
{
	GENERATED_BODY()

	int _hitCounter = 0;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EConstructionResourceType _resourceType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int _hitsToDismantle = 3;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int _amountOfPartsToSpawnWhenDismantled = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AConstructionResourcePieceActorBase> _resourcePieceClass;

	virtual bool IsHittableByActor(AActor* damageCauser) override;
	virtual void TakeHit(AActor* damageCauser, const float damage) override;
};
