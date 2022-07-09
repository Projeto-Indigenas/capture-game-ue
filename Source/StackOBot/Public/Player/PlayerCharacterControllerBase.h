#pragma once

#include "CoreMinimal.h"
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
	FInputAxisBinding _moveVerticalBinding;
	FInputAxisBinding _moveHorizontalBinding;
	FInputActionBinding _primaryAttackBinding;
	FInputActionBinding _evadeAttackBinding;
	
	TWeakObjectPtr<APlayerCharacterBase> _playerCharacter;
	TWeakObjectPtr<APlayerCameraActor> _playerCameraActor;

	void LogOnScreen(const FString& message) const;
	
	void MoveVertical(const float vertical);
	void MoveHorizontal(const float horizontal);
	void PrimaryAttack();
	void EvadeAttack();
	
	void BindInputs();
	void UnbindInputs();

	void TryInitialize(APawn* newPawn);
	void OnCharacterDeath();

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<APlayerCameraActor> _cameraActorClass;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* inPawn) override;
	virtual void OnUnPossess() override;

	virtual void ClientRestart_Implementation(APawn* newPawn) override;
	
public:
	virtual void Tick(float DeltaSeconds) override;
};

