#include "ConstructionResources/ConstructionResourcePieceActorBase.h"

void AConstructionResourcePieceActorBase::BeginPlay()
{
	Super::BeginPlay();

	_staticMeshComponent->SetSimulatePhysics(true);
}

AConstructionResourcePieceActorBase::AConstructionResourcePieceActorBase()
{
	RootComponent = _staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("StaticMesh"));	
}

void AConstructionResourcePieceActorBase::SetSimulatePhysics(const bool simulate) const
{
	_staticMeshComponent->SetSimulatePhysics(simulate);
	
	_staticMeshComponent->SetCollisionEnabled(simulate ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}
