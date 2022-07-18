#include "Constructions/Resources/ConstructionResourceActorBase.h"

#include "Constructions/Resources/ConstructionResourcePieceActorBase.h"
#include "Weapons/Bow/ArrowActorBase.h"

bool AConstructionResourceActorBase::IsHittableByActor(AActor* damageCauser)
{
	return !damageCauser->IsA<AArrowActorBase>();
}

void AConstructionResourceActorBase::TakeHit(AActor* damageCauser, const float damage)
{
	if (++_hitCounter == _hitsToDismantle)
	{
		// one meter up
		const FVector upDisplacement = FVector::UpVector * GetActorScale() * 100.0f;
		const FVector location = GetActorLocation() + upDisplacement;
		const FRotator rotation = GetActorRotation();
		
		for (int index = 0; index < _amountOfPartsToSpawnWhenDismantled; ++index)
		{
			GetWorld()->SpawnActor(_resourcePieceClass, &location, &rotation);
		}

		Destroy();
	}
}
