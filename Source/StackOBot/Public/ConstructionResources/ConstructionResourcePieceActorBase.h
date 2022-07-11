﻿#pragma once

#include "CoreMinimal.h"
#include "ConstructionResourcePieceActorBase.generated.h"

enum class EConstructionResourceType : uint8;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AConstructionResourcePieceActorBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* _staticMeshComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EConstructionResourceType _resourceType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int _pieceValue = 1;

	virtual void BeginPlay() override;
	
public:
	AConstructionResourcePieceActorBase();

	void SetSimulatePhysics(const bool simulate) const;
};
