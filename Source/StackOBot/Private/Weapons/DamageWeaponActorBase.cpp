#include "Weapons/DamageWeaponActorBase.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacterBase.h"

DEFINE_LOG_CATEGORY(DamageWeaponActorBase);

static TAutoConsoleVariable ADamageWeaponActorBaseEnableLogOnScreen(
	TEXT("log.screen.damageweaponactorbase"), false,
	TEXT("AWeaponActorBase class screen logs"),
	ECVF_Default);

void ADamageWeaponActorBase::LogOnScreen(const FString& message) const
{
	const FString debugMessage = FString::Printf(TEXT("%s - %s"), *GetName(), *message);

	if (ADamageWeaponActorBaseEnableLogOnScreen.GetValueOnGameThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, debugMessage, true);
	}

	UE_LOG(DamageWeaponActorBase, Log, TEXT("%s"), *debugMessage)
}

void ADamageWeaponActorBase::OnDamageOtherActor(AActor* damagedActor, UPrimitiveComponent* otherComponent)
{
	IHittable* hittable = Cast<IHittable>(damagedActor);

	if (hittable == nullptr || !hittable->IsHittableByActor(this)) return;
	
	LogOnScreen(FString::Printf(
				TEXT("OnBeginOverlap - applying damage %f, actor being hit %s, actor hitting %s"),
				_damage, *damagedActor->GetName(), *_ownerActor->GetName()));
	
	hittable->TakeHit(_ownerActor.Get(), _damage);
}

void ADamageWeaponActorBase::OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent)
{
	//
}

void ADamageWeaponActorBase::OnBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex,
	bool bFromSweep, const FHitResult& sweepResult)
{
	if (!_ownerActor.IsValid()) return;
	if (_ownerActor == otherActor) return;
	if (_ownerActor == otherActor->GetParentActor()) return;

	OnOverlapAnything(otherActor, otherComp);
	OnDamageOtherActor(otherActor, otherComp);
}
