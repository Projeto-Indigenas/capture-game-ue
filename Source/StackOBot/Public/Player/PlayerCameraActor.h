#pragma once

#include "CoreMinimal.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "Misc/Vectors.h"
#include "PlayerCameraActor.generated.h"

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API APlayerCameraActor : public AActor
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> _targetActor;

	FAcceleratedVector2D _acceleratedLocation;
	
protected:
	UPROPERTY(BlueprintReadWrite)
	USpringArmComponent* _springArmComponent;

	UPROPERTY(BlueprintReadWrite)
	UCameraComponent* _cameraComponent;

	UPROPERTY(EditDefaultsOnly)
	float _acceleration = 1.0f;

	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float deltaSeconds) override;

	void SetTargetActor(AActor* target);
};
