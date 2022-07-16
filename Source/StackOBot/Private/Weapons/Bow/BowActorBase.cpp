#include "Weapons/Bow/BowActorBase.h"

#include "Weapons/Bow/ArrowActorBase.h"

void ABowActorBase::FlyArrowFly(const FVector& direction, const float impulseMultiplier) const
{
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
