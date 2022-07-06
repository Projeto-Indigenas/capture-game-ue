#include "PlayerCharacterControllerBase.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "PlayerCharacterBase.h"

DEFINE_LOG_CATEGORY(PlayerCharacterController);

void APlayerCharacterControllerBase::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);

	UE_LOG(PlayerCharacterController, Log, TEXT("%s"), *debugMessage)
}

void APlayerCharacterControllerBase::MoveVertical(const float vertical)
{
	_directionVector.Y = vertical;
}

void APlayerCharacterControllerBase::MoveHorizontal(const float horizontal)
{
	_directionVector.X = horizontal;
}

void APlayerCharacterControllerBase::BindInputs()
{
	if (_isInputBound || !IsLocalPlayerController()) return;

	_isInputBound = true;

	_moveVerticalBinding = &InputComponent->BindAxis(TEXT("CharacterMoveVertical"), this,
		&APlayerCharacterControllerBase::MoveVertical);
	_moveHorizontalBinding = &InputComponent->BindAxis(TEXT("CharacterMoveHorizontal"), this,
		&APlayerCharacterControllerBase::MoveHorizontal);
}

void APlayerCharacterControllerBase::UnbindInputs()
{
	if (!_isInputBound || !IsLocalPlayerController()) return;

	_isInputBound = false;

	_directionVector = FVector2D::Zero();

	for (FInputAxisBinding& axisBinding : InputComponent->AxisBindings)
	{
		axisBinding.AxisDelegate.Unbind();
	}
	
	InputComponent->AxisBindings.Empty();
}

void APlayerCharacterControllerBase::TryInitialize(APawn* newPawn)
{
	if (_playerCharacter.IsValid())
	{
		LogOnScreen(FString::Printf(TEXT("Already initialized, newPawn %s"),
			IsValid(newPawn) ? *newPawn->GetName() : TEXT("nullptr")));
		
		return;
	}

	LogOnScreen(FString::Printf(TEXT("Trying to initialize, newPawn %s"),
			IsValid(newPawn) ? *newPawn->GetName() : TEXT("nullptr")));
	
	if (APlayerCharacterBase* character = Cast<APlayerCharacterBase>(newPawn))
	{
		_playerCharacter = character;

		if (IsLocalPlayerController())
		{
			if (!IsValid(_playerCameraActor))
			{
				_playerCameraActor = GetWorld()->SpawnActor<APlayerCameraActor>(
					_cameraActorClass, K2_GetActorLocation(), FRotator::ZeroRotator);
			}

			SetViewTarget(_playerCameraActor);
			
			_playerCameraActor->SetTargetActor(character);
		}
	}
}

void APlayerCharacterControllerBase::BeginPlay()
{
	Super::BeginPlay();

	LogOnScreen(FString::Printf(TEXT("BeginPlay called")));

	TryInitialize(GetPawn());
	
	BindInputs();
}

void APlayerCharacterControllerBase::OnPossess(APawn* inPawn)
{
	Super::OnPossess(inPawn);

	TryInitialize(inPawn);
}

void APlayerCharacterControllerBase::OnUnPossess()
{
	Super::OnUnPossess();
}

void APlayerCharacterControllerBase::ClientRestart_Implementation(APawn* newPawn)
{
	Super::ClientRestart_Implementation(newPawn);

	
	LogOnScreen(FString::Printf(TEXT("ClientRestart_Implementation pawn: %s"),
		IsValid(newPawn) ? *newPawn->GetName() : TEXT("nullptr")));

	TryInitialize(newPawn);
}

void APlayerCharacterControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!_playerCharacter.IsValid()) return;
	
	FVector directionVector;
	directionVector.X = _directionVector.Y;
	directionVector.Y = _directionVector.X;

	directionVector = directionVector.GetClampedToMaxSize(1);
	
	if (_directionVector != FVector2D::Zero())
	{
		SetControlRotation(directionVector.Rotation());
	}

	_playerCharacter->AddMovementInput(directionVector, _movementSpeed);
	_playerCharacter->SetMovementSpeed(_directionVector.Length());
}
