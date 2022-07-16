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

void ADamageWeaponActorBase::BeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex,
	bool bFromSweep, const FHitResult& sweepResult)
{
	if (!IsValid(otherActor) || !_ownerActor.IsValid()) return;
	if (otherActor == _ownerActor || otherActor->GetParentActor() == _ownerActor) return;

	OnOverlapAnything(otherActor, otherComp);
	
	if (!otherActor->IsA<APlayerCharacterBase>()) return;
	
	OnDamageOtherActor(otherActor, otherComp);
}

void ADamageWeaponActorBase::OnDamageOtherActor(AActor* damagedActor, UPrimitiveComponent* otherComponent)
{
	LogOnScreen(FString::Printf(
				TEXT("OnBeginOverlap - applying damage %f, actor being hit %s, actor hitting %s"),
				_damage, *damagedActor->GetName(), *_ownerActor->GetName()));
	
	UGameplayStatics::ApplyDamage(damagedActor, _damage, nullptr, _ownerActor.Get(), nullptr);
}

void ADamageWeaponActorBase::OnOverlapAnything(AActor* otherActor, UPrimitiveComponent* otherComponent)
{
	//
}

ADamageWeaponActorBase::ADamageWeaponActorBase() : AWeaponActorBase()
{
	_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	_boxComponent->SetupAttachment(RootComponent);
	_boxComponent->SetGenerateOverlapEvents(false);

	_boxComponent->OnComponentBeginOverlap.AddDynamic(this,
		&ADamageWeaponActorBase::BeginOverlap);
}

void ADamageWeaponActorBase::SetGenerateOverlapEvents(const bool generateOverlapEvents) const
{
	_boxComponent->SetGenerateOverlapEvents(generateOverlapEvents);
}
