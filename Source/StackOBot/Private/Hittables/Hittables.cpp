#include "Hittables/Hittable.h"

bool IHittable::IsHittableByActor(AActor* damageCauser)
{
	return true;
}

void IHittable::TakeHit(AActor* damageCauser, const float damage)
{
	// do nothing
}
