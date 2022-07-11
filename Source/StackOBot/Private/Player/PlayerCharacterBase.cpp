#include "Player/PlayerCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "ConstructionResources/ConstructionResourcePieceActorBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Player/PlayerCharacterControllerBase.h"
#include "Player/Class/DefaultPlayerCharacterClass.h"
#include "Player/Class/PlayerCharacterClassBase.h"
#include "Weapons/WeaponActorBase.h"

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
	if (_playerCharacterClass->PrimaryAttack())
	{
		ReplicatePrimaryAttack_Client();
	}
}

void APlayerCharacterBase::ReplicatePrimaryAttack_Client_Implementation()
{
	_playerCharacterClass->PrimaryAttack();
}

void APlayerCharacterBase::ReplicateEvadeAttack_Server_Implementation()
{
	if (_playerCharacterClass->EvadeAttack())
	{
		// TODO(anderson): replicate evade attack on the simulated clients
	}
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

	TArray<AActor*> childActors;
	GetAllChildActors(childActors);
	AActor** weaponActorPtr = childActors.FindByPredicate([](const AActor* each)
	{
		return IsValid(each) && each->IsA<AWeaponActorBase>();
	});

	if (weaponActorPtr != nullptr)
	{
		_weaponActor = Cast<AWeaponActorBase>(*weaponActorPtr);
	}
	
	UDefaultPlayerCharacterClass* characterClass = NewObject<UDefaultPlayerCharacterClass>(this);
	
	FDefaultCharacterClassInitializationInfo classInfo;
	classInfo.Controller = controller;
	classInfo.Character = this;
	classInfo.Weapon = _weaponActor.Get();
	classInfo.MovementSpeed = _movementSpeed;
	classInfo.MovementSpeedDebuff = _movementSpeedDebuff;	
	
	characterClass->Initialize(classInfo);

	_playerCharacterClass = characterClass;
	
	_currentLifePoints = _initialLifePoints;

	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacterBase::TakeAnyDamage);

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	capsuleComponent->OnComponentBeginOverlap.AddDynamic(this,
		&APlayerCharacterBase::BeginOverlap);
	capsuleComponent->OnComponentEndOverlap.AddDynamic(this,
		&APlayerCharacterBase::EndOverlap);
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
	if (!IsValid(_playerCharacterClass)) return;
	
	if (_playerCharacterClass->PrimaryAttack())
	{
		ReplicatePrimaryAttack_Server();
	}
}

void APlayerCharacterBase::EvadeAttack()
{
	if (_playerCharacterClass->EvadeAttack())
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

void APlayerCharacterBase::PickUpItem()
{
	if (HasAuthority())
	{
		TogglePickItem();

		return;
	}
	
	ReplicatePickUpItem_Server();
}

void APlayerCharacterBase::TakeAnyDamage(AActor* damagedActor, float damage,
                                         const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser)
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

void APlayerCharacterBase::ReplicatePickUpItem_Server_Implementation()
{
	TogglePickItem();
}

void APlayerCharacterBase::ReplicateCarryItem_Clients_Implementation(AConstructionResourcePieceActorBase* piece)
{
	CarryItem(piece);
}

void APlayerCharacterBase::ReplicateDropItem_Clients_Implementation(AConstructionResourcePieceActorBase* piece)
{
	DropItem(piece);
}

void APlayerCharacterBase::TogglePickItem()
{
	if (_carryingPiece.IsValid())
	{
	    AConstructionResourcePieceActorBase* piece = _carryingPiece.Get();
	    
		DropItem(piece);
		
		ReplicateDropItem_Clients(piece);

		return;
	}
	
	if (!_resourcePieceAvailableToPick.IsValid()) return;
	
	AConstructionResourcePieceActorBase* piece = _resourcePieceAvailableToPick.Get();
	
	CarryItem(piece);
	
	ReplicateCarryItem_Clients(piece);
}

void APlayerCharacterBase::CarryItem(AConstructionResourcePieceActorBase* piece)
{
	piece->SetSimulatePhysics(false);
	
	const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
	piece->AttachToActor(this, rules, _resourceItemSocketName);

	FVector socketLocation;
	FRotator socketRotation;
	GetMesh()->GetSocketWorldLocationAndRotation(
		_resourceItemSocketName, socketLocation, socketRotation);
	
	piece->SetActorLocation(socketLocation);
	piece->SetActorRotation(socketRotation + FRotator(0, 0, 90));

	_playerCharacterClass->SetCarryingItem(true);

	_carryingPiece = piece;
	_resourcePieceAvailableToPick.Reset(); 
}

void APlayerCharacterBase::DropItem(AConstructionResourcePieceActorBase* piece)
{
	const FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, false);
	piece->DetachFromActor(rules);

	_playerCharacterClass->SetCarryingItem(false);

	piece->SetSimulatePhysics(true);

	_carryingPiece.Reset();
}

void APlayerCharacterBase::CharacterDied() const
{
	LogOnScreen(TEXT("Setting character to ragdoll on death"));

	_weaponActor->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->DestroyComponent();

	OnCharacterDeath.ExecuteIfBound();
}

void APlayerCharacterBase::BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (!IsValid(otherActor) || otherActor == this) return;
	
	if (!otherActor->IsA<AConstructionResourcePieceActorBase>()) return;

	_resourcePieceAvailableToPick = Cast<AConstructionResourcePieceActorBase>(otherActor);
}

void APlayerCharacterBase::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (_resourcePieceAvailableToPick == otherActor)
	{
		_resourcePieceAvailableToPick.Reset();
	}
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
