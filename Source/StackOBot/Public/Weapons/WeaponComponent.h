#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(WeaponComponent, Log, All);

class UBoxComponent;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class STACKOBOT_API UWeaponComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> _ownerActor;
	
	void LogOnScreen(const FString& message) const;
	
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex,
		bool bFromSweep, const FHitResult& sweepResult);

	/* if needed just uncomment
	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* overlappedComponent, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 otherBodyIndex);
	*/
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float _damage = 5.0f;
	
	virtual void BeginPlay() override;
};
