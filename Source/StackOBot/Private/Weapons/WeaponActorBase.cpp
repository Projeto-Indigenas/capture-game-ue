#include "Weapons/WeaponActorBase.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(WeaponActorBase);

static TAutoConsoleVariable AWeaponActorBaseEnableLogOnScreen(
	TEXT("log.screen.weaponactorbase"), false,
	TEXT("AWeaponActorBase class screen logs"),
	ECVF_Default);

void AWeaponActorBase::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);

	if (AWeaponActorBaseEnableLogOnScreen.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);
	}

	UE_LOG(WeaponActorBase, Log, TEXT("%s"), *debugMessage)
}

void AWeaponActorBase::BeginPlay()
{
	Super::BeginPlay();

	_ownerActor = GetParentActor();
}

AWeaponActorBase::AWeaponActorBase()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Movable);
	_staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	_staticMeshComponent->SetupAttachment(RootComponent);
}
