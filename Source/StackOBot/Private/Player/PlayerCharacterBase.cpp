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
#include "Weapons/DamageWeaponActorBase.h"
#include "Weapons/Bow/BowActorBase.h"

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

void APlayerCharacterBase::ReplicateAimDirection_Server_Implementation(const FVector2D& directionVector)
{
	if (_playerCharacterClass->SetAimDirection(directionVector))
	{
		ReplicateAimDirection_Clients(directionVector);
	}
}

void APlayerCharacterBase::ReplicateAimDirection_Clients_Implementation(const FVector2D& directionVector)
{
	_playerCharacterClass->SetAimDirection(directionVector);	
}

void APlayerCharacterBase::ReplicateMovementDirection_Server_Implementation(const FVector2D& directionVector)
{
	if (_playerCharacterClass->SetMovementDirection(directionVector))
	{
		ReplicateMovementDirection_Clients(directionVector);
	}
}

void APlayerCharacterBase::ReplicateMovementDirection_Clients_Implementation(const FVector2D& directionVector)
{
	if (!IsValid(_playerCharacterClass)) return;
	
	_playerCharacterClass->SetMovementDirection(directionVector);
}

void APlayerCharacterBase::ReplicateRequestJump_Server_Implementation()
{
	if (_playerCharacterClass->Jump())
	{
		ReplicateRequestJump_Clients();
	}
}

void APlayerCharacterBase::ReplicateRequestJump_Clients_Implementation()
{
	_playerCharacterClass->Jump();
}

void APlayerCharacterBase::ReplicatePrimaryAttack_Server_Implementation(const bool pressed)
{
	if (_playerCharacterClass->PrimaryAttack(pressed))
	{
		ReplicatePrimaryAttack_Clients(pressed);
	}
}

void APlayerCharacterBase::ReplicatePrimaryAttack_Clients_Implementation(const bool pressed)
{
	_playerCharacterClass->PrimaryAttack(pressed);
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

ADamageWeaponActorBase* APlayerCharacterBase::GetTheStick_Implementation()
{
	return nullptr;
}

ABowActorBase* APlayerCharacterBase::GetTheBow_Implementation()
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
	_theStickWeapon = GetTheStick();
	_theBowWeapon = GetTheBow();
	
	CreateOrUpdateCharacterClass();

	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacterBase::TakeAnyDamage);
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

void APlayerCharacterBase::SetMovementDirection(const FVector2D& directionVector)
{
	if (_playerCharacterClass->SetMovementDirection(directionVector))
	{
		ReplicateMovementDirection_Server(directionVector);
	}
}

void APlayerCharacterBase::SetAimDirection(const FVector2D& directionVector)
{
	if (_playerCharacterClass->SetAimDirection(directionVector))
	{
		ReplicateAimDirection_Server(directionVector);
	}
}

void APlayerCharacterBase::PrimaryAttack(const bool pressed)
{
	if (!IsValid(_playerCharacterClass)) return;
	
	if (_playerCharacterClass->PrimaryAttack(pressed))
	{
		ReplicatePrimaryAttack_Server(pressed);
	}
}

void APlayerCharacterBase::RequestJump()
{
	if (_playerCharacterClass->Jump())
	{
		ReplicateRequestJump_Server();
	}
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
				_theStickWeapon);
	
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
				_theBowWeapon);

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

void APlayerCharacterBase::TakeAnyDamage(AActor* damagedActor, float damage, const UDamageType* damageType,
	AController* instigatedBy, AActor* damageCauser)
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

void APlayerCharacterBase::CharacterDied() const
{
	LogOnScreen(TEXT("Setting character to ragdoll on death"));

	// TODO(anderson): this should cease to exist when moving the collision box to the character
	_theStickWeapon->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->DestroyComponent();

	OnCharacterDeath.ExecuteIfBound();
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

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacterBase, _currentLifePoints);
}
