#include "PlayerCharacterBase.h"

#include <UnrealNetwork.h>

#include "PlayerCharacterAnimInstanceBase.h"

DEFINE_LOG_CATEGORY(PlayerCharacter);

void APlayerCharacterBase::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);

	UE_LOG(PlayerCharacter, Log, TEXT("%s"), *debugMessage)
}

void APlayerCharacterBase::ReplicateMovementSpeed_Server_Implementation(float speed)
{
	LogOnScreen(FString::Printf(TEXT("ReplicateMovementSpeed_Server speed: %f"), speed));
	
	_movementSpeed = speed;
}

void APlayerCharacterBase::OnReplicateMovementSpeed_Client() const
{
	if (!_animInstance.IsValid())
	{
		LogOnScreen(TEXT("OnReplicateMovementSpeed_Client invalid character anim instance"));
		
		return;
	}

	LogOnScreen(FString::Printf(TEXT("OnReplicateMovementSpeed_Client speed: %f"), _movementSpeed));
	
	_animInstance->SetMovementSpeed(_movementSpeed);
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	LogOnScreen(TEXT("BeginPlay"));

	_animInstance = Cast<UPlayerCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());
}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacterBase, _movementSpeed);
}
