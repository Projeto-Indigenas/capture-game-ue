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
	FVector2D _aimDirectionVector;
	
	TWeakObjectPtr<APlayerCharacterBase> _playerCharacter;
	TWeakObjectPtr<APlayerCameraActor> _playerCameraActor;

	void LogOnScreen(const FString& message) const;
	
	void MoveVertical(const float vertical);
	void MoveHorizontal(const float horizontal);
	void AimVertical(const float vertical);
	void AimHorizontal(const float horizontal);
	void PrimaryAttack(const bool pressed);
	void Jump();

	void PickUpItem();
	
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
	virtual void Tick(float deltaSeconds) override;
	
	APlayerCharacterBase* GetPlayerCharacter() const;
	APlayerCameraActor* GetCameraActor() const;
};

