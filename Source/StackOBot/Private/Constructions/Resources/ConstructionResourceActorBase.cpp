#include "Constructions/Resources/ConstructionResourceActorBase.h"

#include "Constructions/Resources/ConstructionResourcePieceActorBase.h"

void AConstructionResourceActorBase::TakeAnyDamage(AActor* damagedActor, float damage,
                                                   const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser)
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

void AConstructionResourceActorBase::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this,
		&AConstructionResourceActorBase::TakeAnyDamage);
}
