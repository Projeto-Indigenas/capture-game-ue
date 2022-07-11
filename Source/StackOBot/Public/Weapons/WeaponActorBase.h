#pragma once

#include "CoreMinimal.h"
#include "WeaponActorBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(WeaponActorBase, Log, All);

class UStaticMeshComponent;
class UBoxComponent;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AWeaponActorBase : public AActor
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> _ownerActor;

	void LogOnScreen(const FString& message) const;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* _staticMeshComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBoxComponent* _boxComponent;

	UPROPERTY(EditDefaultsOnly)
	float _damage = 5.0f;
	
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex,
		bool bFromSweep, const FHitResult& sweepResult);
	
	virtual void BeginPlay() override;
	
public:
	AWeaponActorBase();

	void SetGenerateOverlapEvents(const bool generateOverlapEvents) const;
};
