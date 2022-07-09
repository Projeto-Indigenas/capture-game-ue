#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MatchGameModeBase.generated.h"

class APlayerCharacterControllerBase;

UCLASS(Abstract, Blueprintable)
class STACKOBOT_API AMatchGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	float _respawnTimeOnPlayerDeath = 5.0;
	
public:
	void OnPlayerDeath(APlayerCharacterControllerBase* playerController);
};
