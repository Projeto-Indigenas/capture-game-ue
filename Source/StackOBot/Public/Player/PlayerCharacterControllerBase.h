#pragma once

#include "CoreMinimal.h"
#include "PlayerCameraActor.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterControllerBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PlayerCharacterController, Log, All);

class APlayerCharacterBase;
class UPlayerCharacterAnimInstanceBase;
class APlayerCameraActor;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API APlayerCharacterControllerBase : public APlayerController
{
	GENERATED_BODY()

	FVector2D _directionVector;

	bool _isInputBound;
	FInputAxisBinding* _moveVerticalBinding;
	FInputAxisBinding* _moveHorizontalBinding;
	
	TWeakObjectPtr<APlayerCharacterBase> _playerCharacter;

	UPROPERTY()
	APlayerCameraActor* _playerCameraActor;

	void LogOnScreen(const FString& message) const;
	
	void MoveVertical(const float vertical);
	void MoveHorizontal(const float horizontal);
	
	void BindInputs();
	void UnbindInputs();

	void TryInitialize(APawn* newPawn);

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<APlayerCameraActor> _cameraActorClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float _movementSpeed = 10.0f;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* inPawn) override;
	virtual void OnUnPossess() override;

	virtual void ClientRestart_Implementation(APawn* newPawn) override;
	
public:
	virtual void Tick(float DeltaSeconds) override;
};

