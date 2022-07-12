#include "Constructions/Building/ConstructionBuildingBase.h"

#include "Net/UnrealNetwork.h"
#include "Components/TextRenderComponent.h"

#include "Constructions/Resources/ConstructionResourceType.h"
#include "Constructions/Resources/ConstructionResourcePieceActorBase.h"

void AConstructionBuildingBase::UpdateResourcesText()
{
	if (!_textRenderComponent.IsValid()) return;
	
	FString infoString = FString::Printf(TEXT("%s house\n"),
		*UEnum::GetDisplayValueAsText(_characterClassType).ToString());

	if (IsResourceful())
	{
		infoString.Append(TEXT("Built and working"));
	}
	else
	{
		for (int index = 0; index < _currentResources.Num(); ++index)
		{
			const FRequiredResource& current = _currentResources[index];
			const FRequiredResource& required = _requiredResourcesToBuild[index];
		
			infoString.Append(*FString::Printf(TEXT(
				"%d/%d %s\n"
			), current.RequiredCount, required.RequiredCount,
			*UEnum::GetDisplayValueAsText(current.ResourceType).ToString()));
		}
	}

	_textRenderComponent->SetText(FText::FromString(infoString));
}

bool AConstructionBuildingBase::IsResourceful()
{
	for (int index = 0; index < _requiredResourcesToBuild.Num(); ++index)
	{
		const int current = _currentResources[index].RequiredCount;
		const int required = _requiredResourcesToBuild[index].RequiredCount;

		if (current < required) return false;
	}
	
	return true;
}

void AConstructionBuildingBase::BeginPlay()
{
	Super::BeginPlay();

	_textRenderComponent = GetBuildingInfoTextRender();

	if (HasAuthority())
	{
		for (const FRequiredResource& res : _requiredResourcesToBuild)
		{
			_currentResources.Add(FRequiredResource(res.ResourceType));
		}
	}
	
	UpdateResourcesText();
}

void AConstructionBuildingBase::OnCurrentResourcesReplicated()
{
	UpdateResourcesText();

	if (IsResourceful())
	{
		// TODO(anderson): now the building can generate class upgrades
	}
}

bool AConstructionBuildingBase::ConsumeResource(AConstructionResourcePieceActorBase* piece)
{
	const EConstructionResourceType resourceType = piece->GetResourceType();

	auto predicate = [resourceType](const FRequiredResource res)
	{
		return res.ResourceType == resourceType;
	};
	
	FRequiredResource* currentPtr = _currentResources.FindByPredicate(predicate);
	const FRequiredResource* requiredPtr = _requiredResourcesToBuild.FindByPredicate(predicate);

	if (currentPtr == nullptr)
	{
		return false;
	}

	FRequiredResource& current = *currentPtr;
	const FRequiredResource& required = *requiredPtr;

	if (current.RequiredCount < required.RequiredCount)
	{
		++current.RequiredCount;

		UpdateResourcesText();

		if (current.RequiredCount == required.RequiredCount)
		{
			// TODO(anderson): now the building can generate class upgrades
		}
		
		return true;
	}

	return false;
}

// will be overriden in blueprints
UTextRenderComponent* AConstructionBuildingBase::GetBuildingInfoTextRender_Implementation()
{
	return nullptr;
}

void AConstructionBuildingBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConstructionBuildingBase, _currentResources);
}
