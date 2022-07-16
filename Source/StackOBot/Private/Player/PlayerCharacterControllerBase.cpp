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

void APlayerCharacterControllerBase::AimVertical(const float vertical)
{
	_aimDirectionVector.X += vertical;
	_aimDirectionVector.Normalize();
}

void APlayerCharacterControllerBase::AimHorizontal(const float horizontal)
{
	_aimDirectionVector.Y += horizontal;
	_aimDirectionVector.Normalize();
}

void APlayerCharacterControllerBase::PrimaryAttack(const bool pressed)
{
	_playerCharacter->PrimaryAttack(pressed);
}

void APlayerCharacterControllerBase::Jump()
{
	_playerCharacter->RequestJump();
}

void APlayerCharacterControllerBase::PickUpItem()
{
	_playerCharacter->PickUpItem();
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

		_playerCharacter = character;
		// the camera actor must exist for this initialization to work properly
		_playerCharacter->Initialize(this);

		_playerCharacter->OnCharacterDeath.BindUObject(this,
			&APlayerCharacterControllerBase::OnCharacterDeath);
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

	InputComponent->BindAxis(TEXT("CharacterMoveVertical"), this,
		&APlayerCharacterControllerBase::MoveVertical);
	InputComponent->BindAxis(TEXT("CharacterMoveHorizontal"), this,
		&APlayerCharacterControllerBase::MoveHorizontal);
	InputComponent->BindAxis(TEXT("CharacterAimVertical"), this,
		&APlayerCharacterControllerBase::AimVertical);
	InputComponent->BindAxis(TEXT("CharacterAimHorizontal"), this,
		&APlayerCharacterControllerBase::AimHorizontal);
	InputComponent->BindAction<TDelegate<void(const bool)>>(TEXT("CharacterPrimaryAttack"), IE_Pressed, this,
		&APlayerCharacterControllerBase::PrimaryAttack, true);
	InputComponent->BindAction<TDelegate<void(const bool)>>(TEXT("CharacterPrimaryAttack"), IE_Released, this,
		&APlayerCharacterControllerBase::PrimaryAttack, false);
	InputComponent->BindAction(TEXT("CharacterJump"), IE_Pressed, this,
		&APlayerCharacterControllerBase::Jump);
	InputComponent->BindAction(TEXT("CharacterCollect"), IE_Pressed, this,
		&APlayerCharacterControllerBase::PickUpItem);
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

void APlayerCharacterControllerBase::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (!IsLocalPlayerController()) return;
	if (!_playerCharacter.IsValid()) return;
	
	_playerCharacter->SetMovementDirection(_directionVector);
	_playerCharacter->SetAimDirection(_aimDirectionVector);
}

APlayerCharacterBase* APlayerCharacterControllerBase::GetPlayerCharacter() const
{
	return _playerCharacter.Get();
}

APlayerCameraActor* APlayerCharacterControllerBase::GetCameraActor() const
{
	return _playerCameraActor.Get();
}
