#include "Weapons/WeaponComponent.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(WeaponComponent);

static TAutoConsoleVariable UWeaponComponentEnableLogOnScreen(
	TEXT("log.screen.weaponcomponent"), false,
	TEXT("UWeaponComponent class screen logs"),
	ECVF_Default);

void UWeaponComponent::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);

	if (UWeaponComponentEnableLogOnScreen.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);
	}

	UE_LOG(WeaponComponent, Log, TEXT("%s"), *debugMessage)
}

void UWeaponComponent::OnBeginOverlap(
	UPrimitiveComponent* overlappedComponent, AActor* otherActor,
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

/* if needed just uncomment
void UWeaponComponent::OnEndOverlap(
	UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{ }
*/

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	_ownerActor = GetOwner();

	SetGenerateOverlapEvents(false);

	OnComponentBeginOverlap.AddDynamic(this, &UWeaponComponent::OnBeginOverlap);
	//OnComponentEndOverlap.AddDynamic(this, &UWeaponComponent::OnEndOverlap);
}
