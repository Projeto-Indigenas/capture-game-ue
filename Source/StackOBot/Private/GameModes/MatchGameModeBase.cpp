#include "GameModes/MatchGameModeBase.h"

#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerCharacterControllerBase.h"

void AMatchGameModeBase::OnPlayerDeath(APlayerCharacterControllerBase* playerController)
{
	TSharedPtr<FTimerHandle> timerHandle = MakeShared<FTimerHandle>();
	GetWorldTimerManager().SetTimer(*timerHandle, FTimerDelegate::CreateLambda([this, playerController, timerHandle]
	{
		UClass* pawnClass = GetDefaultPawnClassForController(playerController);

		if (APawn* pawn = playerController->GetPawn())
		{
			playerController->UnPossess();
			pawn->Destroy();
		}

		const FVector spawnLocation = playerController->GetSpawnLocation();
		const FRotator spawnRotation = FRotator::ZeroRotator;
		if (APawn* newPawn = Cast<APawn>(GetWorld()->SpawnActor(pawnClass, &spawnLocation, &spawnRotation)))
		{
			playerController->Possess(newPawn);
		}
		
		GetWorldTimerManager().ClearTimer(*timerHandle);
	}), _respawnTimeOnPlayerDeath, false);
}
