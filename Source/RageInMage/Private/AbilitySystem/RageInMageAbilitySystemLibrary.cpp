// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RageInMageAbilitySystemTypes.h"
#include "Game/RageInMageGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
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

void URageInMageAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, float Radius,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> OverlappingActors;
		World->OverlapMultiByObjectType(
			OverlappingActors,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams::InitType::AllDynamicObjects,
			FCollisionShape::MakeSphere(Radius),
			SphereParams);
		for (FOverlapResult& Overlap : OverlappingActors)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool URageInMageAbilitySystemLibrary::IsFriendly(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothActorsArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothActorsAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	// Check if both actors share the same team tag (only if team game mode is enabled)
	bool bSameTeam = false;
	if (const ARageInMageGameModeBase* GameMode = Cast<ARageInMageGameModeBase>(UGameplayStatics::GetGameMode(FirstActor)))
	{
		if (GameMode->bIsTeamGame)
		{
			UAbilitySystemComponent* FirstASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(FirstActor);
			UAbilitySystemComponent* SecondASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SecondActor);
			
			if (FirstASC && SecondASC)
			{
				// Get team tags from both actors
				FGameplayTagContainer FirstTeamTags;
				FGameplayTagContainer SecondTeamTags;
				
				FirstASC->GetOwnedGameplayTags(FirstTeamTags);
				SecondASC->GetOwnedGameplayTags(SecondTeamTags);
				
				// Filter to only Team tags
				const FGameplayTag TeamParentTag = FGameplayTag::RequestGameplayTag(FName("Team"));
				FirstTeamTags = FirstTeamTags.Filter(FGameplayTagContainer(TeamParentTag));
				SecondTeamTags = SecondTeamTags.Filter(FGameplayTagContainer(TeamParentTag));
				
				// Check if they share any team tag
				if (FirstTeamTags.Num() > 0 && SecondTeamTags.Num() > 0)
				{
					bSameTeam = FirstTeamTags.HasAny(SecondTeamTags);
				}
			}
		}
	}
	
	const bool bFriendly = bBothActorsArePlayers || bBothActorsAreEnemies || bSameTeam;
	return bFriendly;
}

bool URageInMageAbilitySystemLibrary::IsBothEnemy(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothActorsAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return bBothActorsAreEnemies;
}

FGameplayTagContainer URageInMageAbilitySystemLibrary::GetOwnedGameplayTags(AActor* Actor)
{
	FGameplayTagContainer OwnedGameplayTags;
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		ASC->GetOwnedGameplayTags(OwnedGameplayTags);
	}
	return OwnedGameplayTags;
}
