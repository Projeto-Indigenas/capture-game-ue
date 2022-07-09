#include "Player/PlayerCharacterControllerBase.h"

#include "GameFramework/GameModeBase.h"
#include "GameModes/MatchGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCameraActor.h"

#include "Player/PlayerCharacterBase.h"

DEFINE_LOG_CATEGORY(PlayerCharacterController);

static TAutoConsoleVariable APlayerCharacterControllerBaseEnableLogOnScreen(
	TEXT("log.screen.playercharactercontroller"), false,
	TEXT("APlayerCharacterControllerBase class screen logs"),
	ECVF_Default);

void APlayerCharacterControllerBase::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);

	if (APlayerCharacterControllerBaseEnableLogOnScreen.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);
	}

	UE_LOG(PlayerCharacterController, Log, TEXT("%s"), *debugMessage)
}

void APlayerCharacterControllerBase::MoveVertical(const float vertical)
{
	_directionVector.X = vertical;
}

void APlayerCharacterControllerBase::MoveHorizontal(const float horizontal)
{
	_directionVector.Y = horizontal;
}

void APlayerCharacterControllerBase::PrimaryAttack()
{
	_playerCharacter->PrimaryAttack();
}

void APlayerCharacterControllerBase::EvadeAttack()
{
	_playerCharacter->EvadeAttack();
}

void APlayerCharacterControllerBase::BindInputs()
{
	if (InputComponent == nullptr) return;
	if (_isInputBound || !IsLocalPlayerController()) return;

	_isInputBound = true;

	_moveVerticalBinding = InputComponent->BindAxis(TEXT("CharacterMoveVertical"), this,
		&APlayerCharacterControllerBase::MoveVertical);
	_moveHorizontalBinding = InputComponent->BindAxis(TEXT("CharacterMoveHorizontal"), this,
		&APlayerCharacterControllerBase::MoveHorizontal);
	_primaryAttackBinding = InputComponent->BindAction(TEXT("CharacterPrimaryAttack"), IE_Pressed, this,
		&APlayerCharacterControllerBase::PrimaryAttack);
	_evadeAttackBinding = InputComponent->BindAction(TEXT("CharacterEvadeAttack"), IE_Pressed, this,
		&APlayerCharacterControllerBase::EvadeAttack);
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

	InputComponent->RemoveActionBinding(TEXT("CharacterPrimaryAttack"), IE_Pressed);
	InputComponent->RemoveActionBinding(TEXT("CharacterEvadeAttack"), IE_Pressed);
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

		_playerCharacter->OnCharacterDeath.BindUObject(this,
			&APlayerCharacterControllerBase::OnCharacterDeath);

		if (IsLocalPlayerController())
		{
			if (!_playerCameraActor.IsValid())
			{
				_playerCameraActor = GetWorld()->SpawnActor<APlayerCameraActor>(
					_cameraActorClass, K2_GetActorLocation(), FRotator::ZeroRotator);
			}

			SetViewTarget(_playerCameraActor.Get());
			
			_playerCameraActor->SetTargetActor(character);
		}
	}
}

void APlayerCharacterControllerBase::OnCharacterDeath()
{
	if (_playerCameraActor.IsValid())
	{
		_playerCameraActor->SetTargetActor(nullptr);
	}
	
	if (AMatchGameModeBase* gameMode = Cast<AMatchGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		gameMode->OnPlayerDeath(this);
	}
}

void APlayerCharacterControllerBase::BeginPlay()
{
	Super::BeginPlay();

	LogOnScreen(FString::Printf(TEXT("BeginPlay called")));

	TryInitialize(GetPawn());
}

void APlayerCharacterControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

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

	if (!IsLocalPlayerController()) return;
	
	if (!_playerCharacter.IsValid()) return;
	
	if (_directionVector != FVector2D::Zero())
	{
		const FVector directionVector(_directionVector, 0.0F);	
		SetControlRotation(directionVector.Rotation());
	}
	
	_playerCharacter->SetDirectionVector(_directionVector);
}
