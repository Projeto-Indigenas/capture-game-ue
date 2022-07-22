#include "Weapons/Bow/BowActorBase.h"

#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>

#include "Weapons/Bow/ArrowActorBase.h"

bool ABowActorBase::CanFlyArrow(const float now) const
{
	if (now - _lastArrowFlyTime < _arrowReleaseRateInSeconds) return false;
	return true;
}

bool ABowActorBase::CanFlyArrow() const
{
	return CanFlyArrow(UGameplayStatics::GetTimeSeconds(this));
}

void ABowActorBase::FlyArrowFly(const FVector& direction, const float impulseMultiplier)
{
	const float now = UGameplayStatics::GetTimeSeconds(this);
	if (!CanFlyArrow(now)) return;
	_lastArrowFlyTime = now;
	
	const FVector actorLocation = GetActorLocation();
	const FRotator actorRotation = GetActorRotation();
	AActor* spawnedActor = GetWorld()->SpawnActor(_arrowClass, &actorLocation, &actorRotation);

	if (AArrowActorBase* arrow = Cast<AArrowActorBase>(spawnedActor))
	{
		const FVector impulse = direction * _maxArrowFlyVelocity * impulseMultiplier;

		// this implies that the bow is a child actor of the character actor
		arrow->BeginFlying(_ownerActor.Get(), impulse);
		
		return;
	}

	if (!IsValid(spawnedActor)) return;
	
	spawnedActor->Destroy();
}

void ABowActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABowActorBase, _lastArrowFlyTime);
}
