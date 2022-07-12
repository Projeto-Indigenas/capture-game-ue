#pragma once

#include "CoreMinimal.h"
#include "ConstructionBuildingBase.generated.h"

enum class ECharacterClassType : uint8;
enum class EConstructionResourceType : uint8;
class AConstructionResourcePieceActorBase;
class UTextRenderComponent;

USTRUCT(BlueprintType)
struct STACKOBOT_API FRequiredResource
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EConstructionResourceType ResourceType;

	UPROPERTY(EditDefaultsOnly)
	int RequiredCount = 0;

	FRequiredResource() = default;

	explicit FRequiredResource(const EConstructionResourceType type)
	{
		ResourceType = type;
	}
};

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AConstructionBuildingBase : public AActor
{
	GENERATED_BODY()
	
	TWeakObjectPtr<UTextRenderComponent> _textRenderComponent;

	void UpdateResourcesText();
	bool IsResourceful();
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FRequiredResource> _requiredResourcesToBuild;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ECharacterClassType _characterClassType;

	UPROPERTY(ReplicatedUsing=OnCurrentResourcesReplicated)
	TArray<FRequiredResource> _currentResources;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	UTextRenderComponent* GetBuildingInfoTextRender();

	UFUNCTION()
	void OnCurrentResourcesReplicated();

public:
	bool ConsumeResource(AConstructionResourcePieceActorBase* piece);	
};
