#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterAnimInstanceBase.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PlayerCharacter, Log, All);

class UPlayerCharacterAnimInstanceBase;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

	TWeakObjectPtr<UPlayerCharacterAnimInstanceBase> _animInstance;

	void LogOnScreen(const FString& message) const;
	
protected:
	UPROPERTY(ReplicatedUsing=OnReplicateMovementSpeed_Client)
	float _movementSpeed;

	UFUNCTION(Server, Unreliable)
	void ReplicateMovementSpeed_Server(float speed);
	
	UFUNCTION()
	void OnReplicateMovementSpeed_Client() const;

	virtual void BeginPlay() override;

public:
	void SetMovementSpeed(float speed);
};

inline void APlayerCharacterBase::SetMovementSpeed(float speed)
{
	if (_animInstance->SetMovementSpeed(speed))
	{
		ReplicateMovementSpeed_Server(speed);
	}
}
