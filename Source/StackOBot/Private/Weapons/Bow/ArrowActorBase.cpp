#include "Weapons/Bow/ArrowActorBase.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsSettings.h"

void AArrowActorBase::DisableAndScheduleDestroy()
{
	_isFlying = false;

	_boxComponent->SetComponentTickEnabled(false);
	_boxComponent->SetGenerateOverlapEvents(false);
	_boxComponent->DestroyComponent();
	
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this]
	{
		Destroy();
	}), _delayToDestroy, false);
}

void AArrowActorBase::OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent)
{
	DisableAndScheduleDestroy();
	
	const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
	AttachToComponent(otherComponent, rules);
}

void AArrowActorBase::BeginPlay()
{
	Super::BeginPlay();

	_physicsSettings = GetDefault<UPhysicsSettings>();
}

AArrowActorBase::AArrowActorBase() : ADamageWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	_boxComponent->SetupAttachment(_staticMeshComponent);
	_boxComponent->SetGenerateOverlapEvents(false);

	_boxComponent->OnComponentBeginOverlap.AddDynamic(this,
		&AArrowActorBase::OnBeginOverlap);
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
