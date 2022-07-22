#include "Player/PlayerCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Player/Class/CharacterClassType.h"
#include "Player/PlayerCharacterControllerBase.h"
#include "Player/Class/DefaultPlayerCharacterClass.h"
#include "Player/Class/PlayerCharacterClassBase.h"
#include "Player/Class/ArcherPlayerCharacterClass.h"
#include "Constructions/Resources/ConstructionResourcePieceActorBase.h"
#include "Weapons/Bow/BowActorBase.h"
#include "Weapons/Bow/ArrowActorBase.h"

DEFINE_LOG_CATEGORY(PlayerCharacter);

static TAutoConsoleVariable APlayerCharacterBaseEnableLogOnScreen(
	TEXT("log.screen.playercharacter"), false,
	TEXT("APlayerCharacterBase class screen logs"),
	ECVF_Default);

void APlayerCharacterBase::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);

	if (APlayerCharacterBaseEnableLogOnScreen.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);
	}

	UE_LOG(PlayerCharacter, Log, TEXT("%s"), *debugMessage)
}

void APlayerCharacterBase::ReplicateTakeDamage_Clients_Implementation(float damage)
{
	// TODO(anderson): do something with the damage value
	
	_playerCharacterClass->TakeHit();
}

void APlayerCharacterBase::ReplicateCharacterDeath_Clients_Implementation()
{
	CharacterDied();
}

void APlayerCharacterBase::ReplicatePrimaryAttack_Server_Implementation(const bool pressed)
{
	PrimaryAttack(pressed);
}

void APlayerCharacterBase::ReplicatePrimaryAttack_Clients_Implementation(const bool pressed)
{
	// when running as server and client, should not execute, otherwise, stackoverflow
	if (HasAuthority()) return;
	
	PrimaryAttack(pressed);
}

void APlayerCharacterBase::ReplicateSetMovementDirection_Server_Implementation(const FVector2D& direction)
{
	SetMovementDirection(direction);
}

void APlayerCharacterBase::ReplicateSetMovementDirection_Clients_Implementation(const FVector2D& direction)
{
	// when running as server and client, should not execute, otherwise, stackoverflow
	if (HasAuthority()) return;
	
	SetMovementDirection(direction);
}

void APlayerCharacterBase::ReplicatePickUpItem_Server_Implementation()
{
	PickDropItem();
}

void APlayerCharacterBase::ReplicateCarryItem_Clients_Implementation(AConstructionResourcePieceActorBase* piece)
{
	_playerCharacterClass->CarryItem(piece);
}

void APlayerCharacterBase::ReplicateDropItem_Clients_Implementation(AConstructionResourcePieceActorBase* piece)
{
	_playerCharacterClass->DropItem(piece);
}

void APlayerCharacterBase::CharacterDied() const
{
	LogOnScreen(TEXT("Setting character to ragdoll on death"));

	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->DestroyComponent();

	OnCharacterDeath.ExecuteIfBound();
}

void APlayerCharacterBase::PickDropItem()
{
	_playerCharacterClass->PickDropItem([this](AConstructionResourcePieceActorBase* pickedPiece)
	{
		ReplicateCarryItem_Clients(pickedPiece);
	},
	[this](AConstructionResourcePieceActorBase* droppedPiece)
	{
		ReplicateDropItem_Clients(droppedPiece);
	});
}

void APlayerCharacterBase::CreateOrUpdateCharacterClass()
{
	if (IsValid(_playerCharacterClass) && _playerCharacterClass->GetClassType() == _classType) return;

	// meaning that it is not nullptr and the class has changed
	if (IsValid(_playerCharacterClass))
	{
		_playerCharacterClass->DeInitialize();
	}
	
	switch (_classType)
	{
	case ECharacterClassType::Default:
		{
			UDefaultPlayerCharacterClass* characterClass = NewObject<UDefaultPlayerCharacterClass>(this);

			const FDefaultCharacterClassInitializationInfo classInfo(
				_playerController, this, _movementSpeed,
				_movementSpeedDebuff,
				_lookToDirectionAcceleration,
				_resourceItemSocketName,
				GetTheStick(),
				GetHitBox());
	
			characterClass->Initialize(classInfo);

			_playerCharacterClass = characterClass;
		}
		break;
		
	case ECharacterClassType::Archer:
		{
			UArcherPlayerCharacterClass* characterClass = NewObject<UArcherPlayerCharacterClass>(this);

			const FArcherCharacterClassInitializationInfo classInfo(
				_playerController, this,
				_movementSpeed,
				_movementSpeedDebuff,
				_lookToDirectionAcceleration,
				_resourceItemSocketName,
				GetTheBow());

			characterClass->Initialize(classInfo);

			_playerCharacterClass = characterClass;
		}
		break;
		
	default:
		{
			UE_LOG(PlayerCharacter, Error,
				TEXT("%s not implemented yet"),
				*UEnum::GetDisplayValueAsText(_classType).ToString())
		}
		break;
	}
}

ADamageWeaponActorBase* APlayerCharacterBase::GetTheStick_Implementation()
{
	return nullptr;
}

ABowActorBase* APlayerCharacterBase::GetTheBow_Implementation()
{
	return nullptr;
}

UBoxComponent* APlayerCharacterBase::GetHitBox_Implementation()
{
	return nullptr;
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	_playerController = Cast<APlayerCharacterControllerBase>(GetController());
	Initialize(_playerController.Get());
}

void APlayerCharacterBase::Initialize(APlayerCharacterControllerBase* controller)
{
	if (IsValid(_playerCharacterClass)) return;
	
	LogOnScreen(FString::Printf(
		TEXT("Initialize - initial life points %f"),
		_initialLifePoints));

	_currentLifePoints = _initialLifePoints;

	_playerController = controller;

	CreateOrUpdateCharacterClass();

	_setMovementDirectionSwitcher = TLocalRoleHelper<APlayerCharacterBase, const FVector2D&>()
		.AutonomousProxy(this, &APlayerCharacterBase::ReplicateSetMovementDirection_Server)
		.Authority(this, &APlayerCharacterBase::ReplicateSetMovementDirection_Clients);
	_primaryAttackSwitcher = TLocalRoleHelper<APlayerCharacterBase, const bool>()
		.AutonomousProxy(this, &APlayerCharacterBase::ReplicatePrimaryAttack_Server)
		.Authority(this, &APlayerCharacterBase::ReplicatePrimaryAttack_Clients);
}

void APlayerCharacterBase::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	CreateOrUpdateCharacterClass();
	
	if (!IsValid(_playerCharacterClass)) return;
	
	_playerCharacterClass->Tick(deltaSeconds);
}

void APlayerCharacterBase::Falling()
{
	Super::Falling();

	if (!IsValid(_playerCharacterClass)) return;
	
	_playerCharacterClass->OnFalling();
}

void APlayerCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (const UCharacterMovementComponent* movementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		if (PrevMovementMode == MOVE_Falling && movementComponent->MovementMode == MOVE_Walking)
		{
			_playerCharacterClass->OnLanded();
		}
	}
}

void APlayerCharacterBase::TakeHit(AActor* damageCauser, const float damage)
{
	LogOnScreen(FString::Printf(
		TEXT("TakeDamage - taking %f points, current life %f, damage cause %s, damage receiver %s"),
		damage, _currentLifePoints, *damageCauser->GetName(), *GetName()));
	
	_currentLifePoints = FMath::Max(0, _currentLifePoints -= damage);
	
	_playerCharacterClass->TakeHit();
	
	if (_currentLifePoints > 0)
	{
		ReplicateTakeDamage_Clients(damage);
		
		return;
	}

	ReplicateCharacterDeath_Clients();
	
	CharacterDied();
}

void APlayerCharacterBase::SetMovementDirection(const FVector2D& directionVector) const
{
	if (_playerCharacterClass->SetMovementDirection(directionVector))
	{
		_setMovementDirectionSwitcher.Switch(this, directionVector);
	}
}

void APlayerCharacterBase::SetAimDirection(const FVector2D& directionVector) const
{
	_playerCharacterClass->SetAimDirection(directionVector);
}

void APlayerCharacterBase::PrimaryAttack(const bool pressed) const
{
	if (!IsValid(_playerCharacterClass)) return;

	_playerCharacterClass->PrimaryAttack(pressed, false);

	_primaryAttackSwitcher.Switch(this, pressed);
}

void APlayerCharacterBase::RequestJump() const
{
	_playerCharacterClass->Jump();
}

void APlayerCharacterBase::PickUpItem()
{
	if (HasAuthority())
	{
		PickDropItem();

		return;
	}
	
	ReplicatePickUpItem_Server();
}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacterBase, _currentLifePoints);
}
