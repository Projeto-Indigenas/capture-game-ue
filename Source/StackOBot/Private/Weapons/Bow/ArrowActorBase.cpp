﻿#include "Weapons/Bow/ArrowActorBase.h"

#include <Net/UnrealNetwork.h>

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsSettings.h"

void AArrowActorBase::DisableAndScheduleDestroy()
{
	_isFlying = false;

	_boxComponent->SetComponentTickEnabled(false);
	_boxComponent->SetGenerateOverlapEvents(false);
	_boxComponent->DestroyComponent();
	
	GetWorldTimerManager().SetTimer(_destroyTimerHandle, FTimerDelegate::CreateLambda([this]
	{
		Destroy();
	}), _delayToDestroy, false);
}

void AArrowActorBase::ReplicateDisable_Clients_Implementation(UPrimitiveComponent* otherComponent)
{
	_isFlying = false;

	if (!IsValid(otherComponent)) return;
	
	const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
	AttachToComponent(otherComponent, rules);
}

void AArrowActorBase::OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent)
{
	DisableAndScheduleDestroy();
	
	const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
	AttachToComponent(otherComponent, rules);

	ReplicateDisable_Clients(otherComponent);
}

void AArrowActorBase::BeginPlay()
{
	Super::BeginPlay();

	_physicsSettings = GetDefault<UPhysicsSettings>();

	if (!HasAuthority())
	{
		_boxComponent->OnComponentBeginOverlap.RemoveDynamic(
			this, &AArrowActorBase::OnBeginOverlap);

		_boxComponent->SetGenerateOverlapEvents(false);
		_boxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		_boxComponent->DestroyComponent();
	}
}

void AArrowActorBase::BeginDestroy()
{
	Super::BeginDestroy();

	GetWorldTimerManager().ClearTimer(_destroyTimerHandle);
}

AArrowActorBase::AArrowActorBase() : ADamageWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	_boxComponent->SetupAttachment(_staticMeshComponent);
	_boxComponent->SetGenerateOverlapEvents(false);

	_boxComponent->OnComponentBeginOverlap.AddDynamic(
		this, &AArrowActorBase::OnBeginOverlap);
}

void AArrowActorBase::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (!_isFlying) return;

	const FVector initialLocation = GetActorLocation();
	const FVector finalLocation = initialLocation + _velocity * deltaSeconds;
	SetActorLocation(finalLocation); 
	
	if (!_velocity.IsNearlyZero())
	{
		SetActorRotation(_velocity.Rotation());
	}
}

void AArrowActorBase::BeginFlying(AActor* ownerActor, const FVector& velocity)
{
	_ownerActor = ownerActor;
	
	_boxComponent->SetGenerateOverlapEvents(true);
	
	_velocity = velocity;
	_isFlying = true;
}

void AArrowActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArrowActorBase, _velocity);
	DOREPLIFETIME(AArrowActorBase, _isFlying);
}
