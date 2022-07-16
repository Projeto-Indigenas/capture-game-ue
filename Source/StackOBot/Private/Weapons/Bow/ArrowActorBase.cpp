#include "Weapons/Bow/ArrowActorBase.h"

#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

void AArrowActorBase::DisableAndScheduleDestroy()
{
	_isFlying = false;

	_boxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([this]
	{
		Destroy();
	}), _delayToDestroy, false);
}

void AArrowActorBase::OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent)
{
	const FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
	AttachToComponent(otherComponent, rules);

	DisableAndScheduleDestroy();
}

void AArrowActorBase::BeginPlay()
{
	Super::BeginPlay();

	_physicsSettings = GetDefault<UPhysicsSettings>();
}

AArrowActorBase::AArrowActorBase() : ADamageWeaponActorBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_boxComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	_boxComponent->SetupAttachment(_staticMeshComponent);
}

void AArrowActorBase::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (!_isFlying) return;

	// get the substep value for 5 iterations
	const float substep = deltaSeconds / 5.0f;

	const FVector initialLocation = GetActorLocation();
	// iterate until the forth step
	for (int step = 1; step <= 4; ++step)
	{
		FVector location = initialLocation + _velocity * step * substep;
		SetActorLocation(location);

		// the actor may have hit something in the step
		if (!_isFlying) return;
	}

	// set the last step to the initially expected location 
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
