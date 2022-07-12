#include "Player/Class/PlayerCharacterClassBase.h"

#include "Components/CapsuleComponent.h"

#include "Player/PlayerCharacterAnimInstanceBase.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerCharacterControllerBase.h"
#include "Constructions/Building/ConstructionBuildingBase.h"
#include "Constructions/Resources/ConstructionResourcePieceActorBase.h"

void UPlayerCharacterClassBase::BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (!IsValid(otherActor) || otherActor == _character) return;
	
	if (AConstructionResourcePieceActorBase* piece = Cast<AConstructionResourcePieceActorBase>(otherActor))
	{
		_resourcePieceAvailableToPick = piece;

		return;
	}

	if (AConstructionBuildingBase* building = Cast<AConstructionBuildingBase>(otherActor))
	{
		_constructionBuilding = building;

		// return;
	}
}

void UPlayerCharacterClassBase::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (_resourcePieceAvailableToPick == otherActor)
	{
		_resourcePieceAvailableToPick.Reset();

		return;
	}

	if (_constructionBuilding == otherActor)
	{
		_constructionBuilding.Reset();

		// return;
	}
}

void UPlayerCharacterClassBase::SetCarryingItem(const bool carrying)
{
	_shouldDebuffMovement = carrying;
	_animInstance->SetCarryingItem(carrying);
}

void UPlayerCharacterClassBase::Initialize(const FCharacterClassInitializationInfo& info)
{
	_controller = info.Controller;
	_character = info.Character;
	_movementSpeed = info.MovementSpeed;
	_movementSpeedDebuff = info.MovementSpeedDebuff;
	_resourceItemSocketName = info.ResourceItemSocketName;

	_skeletalMesh = _character->GetMesh();
	UAnimInstance* animInstance = _skeletalMesh->GetAnimInstance();
	_animInstance = Cast<UPlayerCharacterAnimInstanceBase>(animInstance);
	
	_animInstance->SetFalling(_character->bClientWasFalling);

	UCapsuleComponent* capsuleComponent = _character->GetCapsuleComponent();
	capsuleComponent->OnComponentBeginOverlap.AddDynamic(this,
		&UPlayerCharacterClassBase::BeginOverlap);
	capsuleComponent->OnComponentEndOverlap.AddDynamic(this,
		&UPlayerCharacterClassBase::EndOverlap);
}

void UPlayerCharacterClassBase::Tick(float deltaSeconds)
{
	const float movementSpeedDebuff = _shouldDebuffMovement ? _movementSpeedDebuff : 1.0f;
	
	const FVector directionVector = FVector(_directionVector, 0.0f).GetClampedToMaxSize(1);

	if (_controller.IsValid() && _directionVector != FVector2D::ZeroVector)
	{
		_controller->SetControlRotation(directionVector.Rotation());
	}
	
	_character->AddMovementInput(directionVector, _movementSpeed * movementSpeedDebuff);

	_animInstance->SetMovementSpeed(directionVector.Length());
}

void UPlayerCharacterClassBase::OnFalling()
{
	_animInstance->SetFalling(true);
}

void UPlayerCharacterClassBase::OnLanded()
{
	_animInstance->SetJumping(false);
	_animInstance->SetFalling(false);
}

bool UPlayerCharacterClassBase::SetMovementDirection(const FVector2D& directionVector)
{
	if (_directionVector == directionVector) return false;
	
	_directionVector = directionVector;

	return true;
}

FVector2D UPlayerCharacterClassBase::GetMovementDirection() const
{
	return _directionVector;
}

bool UPlayerCharacterClassBase::Jump()
{
	if (!_carryingPiece.IsValid() && _character->CanJump())
	{
		_character->Jump();

		_animInstance->SetJumping(true);

		return true;
	}

	return false;
}

bool UPlayerCharacterClassBase::PrimaryAttack()
{
	if (_animInstance->IsCarryingItem()) return false;
	return _animInstance->PrimaryAttack();
}

bool UPlayerCharacterClassBase::EvadeAttack()
{
	if (_animInstance->IsCarryingItem()) return false;
	return _animInstance->EvadeAttack();
}

bool UPlayerCharacterClassBase::TakeHit()
{
	return _animInstance->TakeHit();
}

void UPlayerCharacterClassBase::PickDropItem(
	const TUniqueFunction<void(AConstructionResourcePieceActorBase*)>& itemPicked,
	const TUniqueFunction<void(AConstructionResourcePieceActorBase*)>& itemDropped)
{
	if (_carryingPiece.IsValid())
	{
		AConstructionResourcePieceActorBase* piece = _carryingPiece.Get();
		
		DropItem(piece);

		if (itemDropped) itemDropped(piece);

		if (_constructionBuilding.IsValid())
		{
			AConstructionBuildingBase* building = _constructionBuilding.Get();

			if (building->ConsumeResource(piece))
			{
				piece->Destroy();
			}
		}

		return;
	}
	
	if (!_resourcePieceAvailableToPick.IsValid()) return;
	
	AConstructionResourcePieceActorBase* piece = _resourcePieceAvailableToPick.Get();
	
	CarryItem(piece);

	if (itemPicked) itemPicked(piece);
}

void UPlayerCharacterClassBase::CarryItem(AConstructionResourcePieceActorBase* piece)
{
	piece->SetSimulatePhysics(false);
	
	const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
	piece->AttachToActor(_character.Get(), rules, _resourceItemSocketName);

	FVector socketLocation;
	FRotator socketRotation;
	_skeletalMesh->GetSocketWorldLocationAndRotation(
		_resourceItemSocketName, socketLocation, socketRotation);
	
	piece->SetActorLocation(socketLocation);
	piece->SetActorRotation(socketRotation + FRotator(0, 0, 90));

	SetCarryingItem(true);

	_carryingPiece = piece;
	_resourcePieceAvailableToPick.Reset(); 
}

void UPlayerCharacterClassBase::DropItem(AConstructionResourcePieceActorBase* piece)
{
	const FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, false);
	piece->DetachFromActor(rules);

	SetCarryingItem(false);

	piece->SetSimulatePhysics(true);

	if (piece->IsOverlappingActor(_character.Get()))
	{
		_resourcePieceAvailableToPick = piece;
	}

	_carryingPiece.Reset();
}
