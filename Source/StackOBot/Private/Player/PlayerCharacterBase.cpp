#include "Player/PlayerCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Player/PlayerCharacterControllerBase.h"
#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/Behaviours/DefaultPlayerCharacterClass.h"
#include "Player/Behaviours/PlayerCharacterClassBase.h"
#include "Weapons/WeaponComponent.h"

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

void APlayerCharacterBase::ReplicatePrimaryAttack_Server_Implementation()
{
	auto enableCollision = [this]
	{
		if (_weaponComponent.IsValid())
		{
			_weaponComponent->SetGenerateOverlapEvents(true);
		}
	};

	if (!IsRunningDedicatedServer())
	{
		enableCollision();

		return;
	}
	
	if (!_animInstance.IsValid()) return;

	if (_animInstance->PrimaryAttack())
	{
		enableCollision();	
		
		ReplicatePrimaryAttack_Client();
	}
}

void APlayerCharacterBase::ReplicatePrimaryAttack_Client_Implementation()
{
	if (IsLocallyControlled()) return; 
	
	if (!_animInstance.IsValid()) return;

	_animInstance->PrimaryAttack();
}

void APlayerCharacterBase::ReplicateEvadeAttack_Server_Implementation()
{
	if (!_animInstance.IsValid()) return;

	_animInstance->EvadeAttack();
}

void APlayerCharacterBase::SetAnimMovementSpeed(float speed) const
{
	if (!_animInstance.IsValid()) return;
	
	_animInstance->SetMovementSpeed(speed);
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	Initialize(Cast<APlayerCharacterControllerBase>(GetController()));
}

void APlayerCharacterBase::Initialize(APlayerCharacterControllerBase* controller)
{
	if (IsValid(_playerCharacterClass)) return;
	
	LogOnScreen(FString::Printf(
		TEXT("Initialize - initial life points %f"),
		_initialLifePoints));

	_playerCharacterClass = NewObject<UDefaultPlayerCharacterClass>(this);
	
	FCharacterClassInitializationInfo classInfo;
	classInfo.Controller = controller;
	classInfo.Character = this;
	classInfo.MovementSpeed = _movementSpeed;
	
	_playerCharacterClass->Initialize(classInfo);
	
	_currentLifePoints = _initialLifePoints;

	_weaponComponent = Cast<UWeaponComponent>(GetComponentByClass(UWeaponComponent::StaticClass()));
	_animInstance = Cast<UPlayerCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());

	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacterBase::TakeAnyDamage);
		
	if (!_animInstance.IsValid())
	{
		LogOnScreen(TEXT("BeginPlay - Invalid anim instance"));

		return;
	}

	if (!_weaponComponent.IsValid())
	{
		LogOnScreen(TEXT("BeginPlay - Invalid weapon component"));
	}

	_animInstance->PrimaryAttackFinished = [this]
	{
		_weaponComponent->SetGenerateOverlapEvents(false);
	};
}

void APlayerCharacterBase::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

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

void APlayerCharacterBase::PrimaryAttack()
{
	if (!_animInstance.IsValid()) return;

	if (_animInstance->PrimaryAttack())
	{
		ReplicatePrimaryAttack_Server();
	}
}

void APlayerCharacterBase::EvadeAttack()
{
	if (!_animInstance.IsValid()) return;

	if (_animInstance->EvadeAttack())
	{
		ReplicateEvadeAttack_Server();
	}
}

void APlayerCharacterBase::RequestJump()
{
	if (_playerCharacterClass->Jump())
	{
		ReplicateRequestJump_Server();
	}
}

void APlayerCharacterBase::TakeAnyDamage(AActor* DamagedActor, float damage, const UDamageType* DamageType,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
	LogOnScreen(FString::Printf(
		TEXT("TakeDamage - taking %f points, current life %f, damage cause %s, damage receiver %s"),
		damage, _currentLifePoints, *DamageCauser->GetName(), *GetName()));
	
	_currentLifePoints = FMath::Max(0, _currentLifePoints -= damage);
	
	ReplicateTakeDamage_Client(damage);

	if (_animInstance.IsValid())
	{
		_animInstance->TakeHit();
	}
	
	if (_currentLifePoints > 0) return;

	LogOnScreen(TEXT("Setting character to ragdoll on death"));

	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->DestroyComponent();

	OnCharacterDeath.ExecuteIfBound();
}

void APlayerCharacterBase::ReplicateTakeDamage_Client_Implementation(float damage)
{
	if (!_animInstance.IsValid()) return;

	// TODO(anderson): do something with the damage value
	
	_animInstance->TakeHit();
}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacterBase, _currentLifePoints);
}
