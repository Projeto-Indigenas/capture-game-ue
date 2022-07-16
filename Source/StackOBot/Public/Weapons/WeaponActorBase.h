#pragma once

#include "CoreMinimal.h"
#include "WeaponActorBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(WeaponActorBase, Log, All);

class UStaticMeshComponent;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AWeaponActorBase : public AActor
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<AActor> _ownerActor;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* _staticMeshComponent;
	
	virtual void LogOnScreen(const FString& message) const;
	
	virtual void BeginPlay() override;
	
public:
	AWeaponActorBase();
};
