#include "Weapons/WeaponActorBase.h"

#include "Components/BoxComponent.h"
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

void AWeaponActorBase::BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex,
	bool bFromSweep, const FHitResult& sweepResult)
{
	if (!IsValid(otherActor) || !_ownerActor.IsValid()) return;
	if (otherActor == _ownerActor) return;

	LogOnScreen(FString::Printf(
			TEXT("OnBeginOverlap - applying damage %f, actor being hit %s, actor hitting %s"),
			_damage, *otherActor->GetName(), *_ownerActor->GetName()));

	UGameplayStatics::ApplyDamage(otherActor, _damage, nullptr, _ownerActor.Get(), nullptr);

	SetGenerateOverlapEvents(false);
}

void AWeaponActorBase::BeginPlay()
{
	Super::BeginPlay();

	_ownerActor = GetParentActor();

	_boxComponent->OnComponentBeginOverlap.AddDynamic(this,
		&AWeaponActorBase::BeginOverlap);
}

AWeaponActorBase::AWeaponActorBase()
{
	_staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = _staticMeshComponent;
	
	_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	_boxComponent->SetupAttachment(RootComponent);
	_boxComponent->SetGenerateOverlapEvents(false);
}

void AWeaponActorBase::SetGenerateOverlapEvents(const bool generateOverlapEvents) const
{
	_boxComponent->SetGenerateOverlapEvents(generateOverlapEvents);
}
