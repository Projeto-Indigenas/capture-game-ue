#include "PlayerCameraActor.h"

void APlayerCameraActor::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Super::BeginPlay();

	_acceleratedLocation.SetAcceleration(_acceleration);
}

void APlayerCameraActor::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (!_targetActor.IsValid()) return;

	_acceleratedLocation.Tick(deltaSeconds);
	
	_acceleratedLocation = _targetActor->GetActorLocation();

	SetActorLocation(_acceleratedLocation);
}

void APlayerCameraActor::SetTargetActor(AActor* target)
{
	_targetActor = target;

	if (_targetActor.IsValid())
	{
		_acceleratedLocation.SetCurrent(_targetActor->GetActorLocation());
	}
}
