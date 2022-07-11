﻿#include "ConstructionResources/ConstructionResourcePieceActorBase.h"

void AConstructionResourcePieceActorBase::BeginPlay()
{
	Super::BeginPlay();

	SetSimulatePhysics(true);
}

AConstructionResourcePieceActorBase::AConstructionResourcePieceActorBase()
{
	RootComponent = _staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("StaticMesh"));	
}

void AConstructionResourcePieceActorBase::SetSimulatePhysics(const bool simulate) const
{
	_staticMeshComponent->SetSimulatePhysics(simulate);

	const auto collision = simulate ? ECollisionEnabled::PhysicsOnly : ECollisionEnabled::NoCollision;
	_staticMeshComponent->SetCollisionEnabled(collision);
}
