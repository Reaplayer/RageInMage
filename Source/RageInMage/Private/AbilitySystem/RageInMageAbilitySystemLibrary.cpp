// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"
#include "RageInMageAbilitySystemTypes.h"
#include "Game/RageInMageGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/MageWidgetController.h"
#include "Player/MagePlayerState.h"
#include "RageInMage/RageInMageGameMode.h"
#include "UI/HUD/MageHUD.h"

UOverlayWidgetController* URageInMageAbilitySystemLibrary::GetOverlayWidgetController(UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMageHUD* MageHUD = Cast<AMageHUD>(PC->GetHUD()))
		{
			AMagePlayerState* PS = PC->GetPlayerState<AMagePlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MageHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* URageInMageAbilitySystemLibrary::GetAttributeMenuWidgetController(
	UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMageHUD* MageHUD = Cast<AMageHUD>(PC->GetHUD()))
		{
			AMagePlayerState* PS = PC->GetPlayerState<AMagePlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return MageHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void URageInMageAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	ARageInMageGameModeBase* GameMode = Cast<ARageInMageGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!GameMode) return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GameMode->CharacterClassInfo;
	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void URageInMageAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject,
	UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilityClass);
	}

	const FCharacterClassDefaultInfo& CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassDefaultInfo.StartupAbilities)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor()))
		{
			FGameplayAbilitySpec(AbilityClass, CombatInterface->GetPlayerLevel());
			ASC->GiveAbility(AbilityClass);
		}
	}
	
}

UCharacterClassInfo* URageInMageAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	ARageInMageGameModeBase* GameMode = Cast<ARageInMageGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!GameMode) return nullptr;
	return GameMode->CharacterClassInfo;
}

bool URageInMageAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FRageInMageGameplayEffectContext* RInMEffectContext = static_cast<const FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return RInMEffectContext->IsCriticalHit();
	}
	return false;
}

bool URageInMageAbilitySystemLibrary::IsVulnerableHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FRageInMageGameplayEffectContext* RInMEffectContext = static_cast<const FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return RInMEffectContext->IsVulnerableHit();
	}
	return false;
}

bool URageInMageAbilitySystemLibrary::IsResistantHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FRageInMageGameplayEffectContext* RInMEffectContext = static_cast<const FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return RInMEffectContext->IsResistantHit();
	}
	return false;
}

void URageInMageAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
                                                       bool bInIsCritHit)
{
	if (FRageInMageGameplayEffectContext* RInMEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RInMEffectContext->SetIsCriticalHit(bInIsCritHit);
	}
}

void URageInMageAbilitySystemLibrary::SetIsVulnerableHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsVulnerableHit)
{
	if (FRageInMageGameplayEffectContext* RInMEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RInMEffectContext->SetIsVulnerableHit(bInIsVulnerableHit);
	}
}

void URageInMageAbilitySystemLibrary::SetIsResistantHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsResistantHit)
{
	if (FRageInMageGameplayEffectContext* RInMEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RInMEffectContext->SetIsResistantHit(bIsResistantHit);
	}
}
