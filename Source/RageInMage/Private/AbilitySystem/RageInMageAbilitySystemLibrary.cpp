// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RageInMageAbilitySystemTypes.h"
#include "AbilitySystem/Data/ConditionInfo.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/KeyIconData.h"
#include "Character/RageInMageCharacterBase.h"
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/DataTable.h"
#include "Game/RageInMageGameModeBase.h"
#include "Input/RageInMageConfig.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RageInMageGameplayTag.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "Game/RageInMageGameState.h"
#include "Player/RageInMagePlayerState.h"
#include "RageInMage/RageInMageLogChannels.h"
#include "Styling/SlateColor.h"
#include "UI/HUD/RageInMageHUD.h"
#include "UI/WidgetController/SettingsWidgetController.h"
#include "UI/WidgetController/InventoryWidgetController.h"

bool URageInMageAbilitySystemLibrary::MakeGASReferences(
	APlayerController* PC, FPlayerGASReferences& OutGASRefs, ARageInMageHUD*& OutRageHUD)
{
	OutRageHUD = Cast<ARageInMageHUD>(PC->GetHUD());
	if (OutRageHUD)
		{
			ARageInMagePlayerState* PS = PC->GetPlayerState<ARageInMagePlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutGASRefs = FPlayerGASReferences(PC, PS, ASC, AS);
			return true;
		}
	return false;
}

USpellMenuWidgetController* URageInMageAbilitySystemLibrary::GetSpellMenuWidgetController(
	APlayerController* PC)
{
	FPlayerGASReferences GASRefs;
	ARageInMageHUD* RageHUD = nullptr;
	if (MakeGASReferences(PC, GASRefs, RageHUD))
	{
		return RageHUD->GetSpellMenuWidgetController(PC);
	}
	return nullptr;
}

UAttributeMenuWidgetController* URageInMageAbilitySystemLibrary::GetAttributeMenuWidgetController(
	APlayerController* PC)
{
	FPlayerGASReferences GASRefs;
	ARageInMageHUD* RageHUD = nullptr;
	if (MakeGASReferences(PC, GASRefs, RageHUD))
	{
		return RageHUD->GetAttributeMenuWidgetController(PC);
	}
	return nullptr;
}

UOverlayWidgetController* URageInMageAbilitySystemLibrary::GetOverlayWidgetController(
	APlayerController* PC)
{
	FPlayerGASReferences GASRefs;
	ARageInMageHUD* RageHUD = nullptr;
	if (MakeGASReferences(PC, GASRefs, RageHUD))
	{
		return RageHUD->GetOverlayWidgetController(PC);
	}
	return nullptr;
}

USettingsWidgetController* URageInMageAbilitySystemLibrary::GetSettingsWidgetController(
	APlayerController* PC)
{
	FPlayerGASReferences GASRefs;
	ARageInMageHUD* RageHUD = nullptr;
	if (MakeGASReferences(PC, GASRefs, RageHUD))
	{
		return RageHUD->GetSettingsWidgetController(PC);
	}
	return nullptr;
}

void URageInMageAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
                                                                  ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	// Retrieve the GameState cast to our custom class
	ARageInMageGameState* GameState = Cast<ARageInMageGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	if (!GameState)
	{
		UE_LOG(LogRageInMage, Error, TEXT("GameState is null"));
	}
	if (!GameState->CharacterClassInfo)
	{
		UE_LOG(LogRageInMage, Error, TEXT("CharacterClassInfo is null"));
		return;
	}

	// Retrieve Avatar Actor
	AActor* AvatarActor = ASC->GetAvatarActor();

	// Retrieve Character Class Info
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

	// Attach Primary Attributes
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	// Attach Secondary Attributes
	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	// Attach Vital Attributes
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
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilityClass);
		}
	}
	
}

UCharacterClassInfo* URageInMageAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	// Try to get the info from the GameState
	if (ARageInMageGameState* GameState = Cast<ARageInMageGameState>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		if (GameState->CharacterClassInfo)
		{
			return GameState->CharacterClassInfo;
		}
	}

	return nullptr;
}

UConditionInfo* URageInMageAbilitySystemLibrary::GetConditionInfo(const UObject* WorldContextObject)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo && ClassInfo->ConditionInfo)
	{
		return ClassInfo->ConditionInfo;
	}
	return nullptr;
}

bool URageInMageAbilitySystemLibrary::ApplyConditionToTarget(
	UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
	const FGameplayTag& ConditionTag, const UObject* WorldContextObject)
{
	if (!SourceASC || !TargetASC) return false;

	UConditionInfo* ConditionInfoData = GetConditionInfo(WorldContextObject);
	if (!ConditionInfoData) return false;

	const FRageInMageConditionInfo Info = ConditionInfoData->FindConditionInfoForTag(ConditionTag, true);
	if (!Info.ConditionTag.IsValid() || !Info.ConditionEffect) return false;

	// Check if blocked by existing conditions on target
	FGameplayTagContainer OwnedTags;
	TargetASC->GetOwnedGameplayTags(OwnedTags);
	if (Info.BlockedByConditions.Num() > 0 && OwnedTags.HasAny(Info.BlockedByConditions))
	{
		return false;
	}

	// Remove conditions this one overrides
	if (Info.OverridesConditions.Num() > 0)
	{
		TargetASC->RemoveActiveEffectsWithGrantedTags(Info.OverridesConditions);
	}

	// Apply the condition GE
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceASC->GetAvatarActor());
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(Info.ConditionEffect, 1, ContextHandle);

	if (SpecHandle.IsValid())
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ConditionTag, Info.BaseIntensity);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		// Treat this condition as a stun: grant immunity for its own duration plus the grace period,
		// starting now rather than on natural expiry, so a refreshed/extended stun can't outrun it.
		if (Info.StunImmunityGraceSeconds > 0.f)
		{
			ApplyStunImmunity(SourceASC, TargetASC, WorldContextObject, Info.BaseIntensity + Info.StunImmunityGraceSeconds);
		}

		return true;
	}

	return false;
}

void URageInMageAbilitySystemLibrary::ApplyStunImmunity(
	UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
	const UObject* WorldContextObject, float TotalDuration)
{
	if (!SourceASC || !TargetASC || TotalDuration <= 0.f) return;

	UConditionInfo* ConditionInfoData = GetConditionInfo(WorldContextObject);
	if (!ConditionInfoData || !ConditionInfoData->StunImmunityEffect) return;

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceASC->GetAvatarActor());
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ConditionInfoData->StunImmunityEffect, 1, ContextHandle);
	if (!SpecHandle.IsValid()) return;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, FRageInMageGameplayTag::Get().Condition_StunImmune, TotalDuration);
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void URageInMageAbilitySystemLibrary::ApplyGuaranteedStun(
	UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
	const UObject* WorldContextObject, float StunDuration, float GraceSeconds)
{
	if (!SourceASC || !TargetASC || StunDuration <= 0.f) return;

	UConditionInfo* ConditionInfoData = GetConditionInfo(WorldContextObject);
	if (!ConditionInfoData || !ConditionInfoData->StunnedEffect) return;

	// Strip any existing immunity so this guaranteed stun always lands, regardless of source.
	TargetASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FRageInMageGameplayTag::Get().Condition_StunImmune));

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceASC->GetAvatarActor());
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ConditionInfoData->StunnedEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle, FRageInMageGameplayTag::Get().Condition_Stunned, StunDuration);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	ApplyStunImmunity(SourceASC, TargetASC, WorldContextObject, StunDuration + GraceSeconds);
}

void URageInMageAbilitySystemLibrary::ApplyChainLightningDamage(
	AActor* InstigatorActor,
	AActor* InitialTargetActor,
	const FGameplayEffectSpecHandle& DamageEffectSpecHandle,
	float JumpRadius,
	float DamageFalloffPerJump,
	int32 MaxJumps,
	UNiagaraSystem* ImpactEffect)
{
	if (!InstigatorActor || !InitialTargetActor || !DamageEffectSpecHandle.IsValid()) return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);
	if (!SourceASC) return;

	const FRageInMageGameplayTag& GameplayTags = FRageInMageGameplayTag::Get();
	const FGameplayEffectSpec* OriginalSpec = DamageEffectSpecHandle.Data.Get();

	TArray<AActor*> AlreadyHit;
	AlreadyHit.Add(InitialTargetActor);

	AActor* CurrentTarget = InitialTargetActor;
	float CurrentMultiplier = 1.f;

	for (int32 JumpIndex = 0; JumpIndex <= MaxJumps; ++JumpIndex)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CurrentTarget);
		if (!TargetASC) break;

		// Build a scaled copy of the spec for this hit (falloff compounds per jump).
		FGameplayEffectSpecHandle ScaledHandle(new FGameplayEffectSpec(*OriginalSpec));
		FGameplayEffectSpec* ScaledSpec = ScaledHandle.Data.Get();
		if (!FMath::IsNearlyEqual(CurrentMultiplier, 1.f))
		{
			for (TPair<FGameplayTag, float>& Pair : ScaledSpec->SetByCallerTagMagnitudes)
			{
				Pair.Value *= CurrentMultiplier;
			}
		}

		SourceASC->ApplyGameplayEffectSpecToTarget(*ScaledSpec, TargetASC);

		FGameplayTagContainer CurrentOwnedTags;
		TargetASC->GetOwnedGameplayTags(CurrentOwnedTags);
		const bool bIsCharged = CurrentOwnedTags.HasTag(GameplayTags.Condition_Charged);
		const bool bIsOverCharged = CurrentOwnedTags.HasTag(GameplayTags.Condition_OverCharged);
		{
			const URageInMageAttributeSet* TargetAS = TargetASC->GetSet<URageInMageAttributeSet>();
			const float TargetCharge = TargetAS ? TargetAS->GetCharge() : -1.f;
			const FString Msg = FString::Printf(TEXT("[ChainLightning] Jump %d -> %s | Charge=%.1f | Charged=%s OverCharged=%s"),
				JumpIndex, *GetNameSafe(CurrentTarget), TargetCharge,
				bIsCharged ? TEXT("Yes") : TEXT("No"), bIsOverCharged ? TEXT("Yes") : TEXT("No"));
			UE_LOG(LogRageInMage, Log, TEXT("%s"), *Msg);
			if (GEngine)
			{
				// Silenced — uncomment to restore the on-screen chain readout (UE_LOG above still fires).
				// GEngine->AddOnScreenDebugMessage(-1, 4.f, bIsOverCharged ? FColor::Red : (bIsCharged ? FColor::Cyan : FColor::White), Msg);
			}
		}

		if (JumpIndex >= MaxJumps) break;

		// The hit we just landed only arcs onward if it left CurrentTarget Charged (>= 50) —
		// whether they already were, or this very hit's Charge gain just pushed them there.
		if (!bIsCharged)
		{
			const FString Msg = FString::Printf(TEXT("[ChainLightning] Stopped at %s — not Charged, no further jump"), *GetNameSafe(CurrentTarget));
			UE_LOG(LogRageInMage, Log, TEXT("%s"), *Msg);
			if (GEngine)
			{
				// Silenced — uncomment to restore the on-screen chain readout (UE_LOG above still fires).
				// GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, Msg);
			}
			break;
		}

		// Debug: visualize the actual search radius used to find the next jump target.
		// Silenced — uncomment to restore the per-jump search-radius sphere.
#if ENABLE_DRAW_DEBUG
		// DrawDebugSphere(InstigatorActor->GetWorld(), CurrentTarget->GetActorLocation(), JumpRadius, 24, FColor::Cyan, false, 3.f, 0, 2.f);
#endif

		// Find the nearest not-yet-hit enemy within JumpRadius — eligibility to RECEIVE a jump
		// no longer depends on the candidate's own Charge; only the source needs to be Charged.
		TArray<AActor*> NearbyActors;
		GetLivePlayersWithinRadius(InstigatorActor, JumpRadius, NearbyActors, AlreadyHit, CurrentTarget->GetActorLocation());

		AActor* NextTarget = nullptr;
		float NearestDistSq = TNumericLimits<float>::Max();
		for (AActor* Candidate : NearbyActors)
		{
			if (!Candidate || AlreadyHit.Contains(Candidate)) continue;
			if (IsBothEnemy(InstigatorActor, Candidate)) continue;

			const float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Candidate->GetActorLocation());
			if (DistSq < NearestDistSq)
			{
				NearestDistSq = DistSq;
				NextTarget = Candidate;
			}
		}

		if (!NextTarget)
		{
			const FString Msg = FString::Printf(TEXT("[ChainLightning] Stopped at %s — Charged but no eligible target in range"), *GetNameSafe(CurrentTarget));
			UE_LOG(LogRageInMage, Log, TEXT("%s"), *Msg);
			if (GEngine)
			{
				// Silenced — uncomment to restore the on-screen chain readout (UE_LOG above still fires).
				// GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, Msg);
			}
			break;
		}

		// Spawn the jump beam from the target we just left to the one we're chaining to — same
		// NS_LightningShock convention as the rod VFX: spawn at the source point, then feed the
		// target's absolute world position into the system's BeamEnd user parameter (BeamEnd is
		// not relative/distance-based, so no separate distance calculation is needed here).
		if (ImpactEffect)
		{
			UNiagaraComponent* JumpVFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				InstigatorActor, ImpactEffect, CurrentTarget->GetActorLocation(), FRotator::ZeroRotator);
			if (JumpVFX)
			{
				JumpVFX->SetVariablePosition(FName("BeamEnd"), NextTarget->GetActorLocation());
			}
		}

		AlreadyHit.Add(NextTarget);
		// Chaining out of an OverCharged (>= 100) target ignores cumulative falloff entirely.
		CurrentMultiplier = bIsOverCharged
			? 1.f
			: CurrentMultiplier * (1.f - DamageFalloffPerJump);
		CurrentTarget = NextTarget;
	}
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
	if (const FRageInMageGameplayEffectContext* RageEffectContext = static_cast<const FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return RageEffectContext->IsResistantHit();
	}
	return false;
}

void URageInMageAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
                                                       bool bInIsCritHit)
{
	if (FRageInMageGameplayEffectContext* RageEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RageEffectContext->SetIsCriticalHit(bInIsCritHit);
	}
}

void URageInMageAbilitySystemLibrary::SetIsVulnerableHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsVulnerableHit)
{
	if (FRageInMageGameplayEffectContext* RageEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RageEffectContext->SetIsVulnerableHit(bInIsVulnerableHit);
	}
}

void URageInMageAbilitySystemLibrary::SetIsResistantHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsResistantHit)
{
	if (FRageInMageGameplayEffectContext* RageEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RageEffectContext->SetIsResistantHit(bIsResistantHit);
	}
}

bool URageInMageAbilitySystemLibrary::IsIceDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FRageInMageGameplayEffectContext* RageEffectContext = static_cast<const FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return RageEffectContext->IsIceDamage();
	}
	return false;
}

void URageInMageAbilitySystemLibrary::SetIsIceDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsIceDamage)
{
	if (FRageInMageGameplayEffectContext* RageEffectContext = static_cast<FRageInMageGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		RageEffectContext->SetIsIceDamage(bInIsIceDamage);
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
	if (!FirstActor || !SecondActor) return false;
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

int32 URageInMageAbilitySystemLibrary::GetXPRewardForClassAndLevel(ECharacterClass CharacterClass, int32 Level,
	const UObject* WorldContextObject)
{
	// Retrieve the GameState cast to our custom class
	ARageInMageGameState* GameState = Cast<ARageInMageGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	if (!GameState || !GameState->CharacterClassInfo) return 0;

	// Retrieve Character Class Info
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo) return 0;
	const FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	const float XPReward = CharacterClassDefaultInfo.XPReward.GetValueAtLevel(Level);
	return FMath::RoundToInt(XPReward);
}

int32 URageInMageAbilitySystemLibrary::GetLocalPlayerIndex(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null"));
		return -1;
	}


	auto LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayer is null"));
		return -1;
	}

	return LocalPlayer->GetControllerId();
}

UInventoryWidgetController* URageInMageAbilitySystemLibrary::GetInventoryWidgetController(
	APlayerController* PC)
{
	FPlayerGASReferences GASRefs;
	ARageInMageHUD* RageHUD = nullptr;
	if (MakeGASReferences(PC, GASRefs, RageHUD))
	{
		return RageHUD->GetInventoryWidgetController(PC);
	}
	return nullptr;
}

// ──────────────────────────────────────────
// Aim Prediction Utilities
// ──────────────────────────────────────────

TArray<FVector> URageInMageAbilitySystemLibrary::CalculateProjectileArcPoints(
	const UObject* WorldContextObject,
	const FVector& LaunchLocation,
	const FVector& TargetLocation,
	float ArcParam,
	int32 NumPoints)
{
	TArray<FVector> Points;

	FVector LaunchVelocity;
	const bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		WorldContextObject, LaunchVelocity, LaunchLocation, TargetLocation, 0, ArcParam);

	if (!bHaveSolution)
	{
		Points.Add(LaunchLocation);
		Points.Add(TargetLocation);
		return Points;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		Points.Add(LaunchLocation);
		Points.Add(TargetLocation);
		return Points;
	}

	const float GravityZ = World->GetGravityZ();

	// Estimate flight time from horizontal distance and speed
	const FVector HorizontalVelocity(LaunchVelocity.X, LaunchVelocity.Y, 0.f);
	const float HorizontalSpeed = HorizontalVelocity.Size();
	const float HorizontalDist = FVector::Dist2D(LaunchLocation, TargetLocation);
	const float EstimatedFlightTime = (HorizontalSpeed > KINDA_SMALL_NUMBER)
		? HorizontalDist / HorizontalSpeed : 1.0f;

	NumPoints = FMath::Max(NumPoints, 2);
	const float TimeStep = EstimatedFlightTime / (NumPoints - 1);

	for (int32 i = 0; i < NumPoints; ++i)
	{
		const float T = TimeStep * i;
		// P(t) = P0 + V*t + 0.5*g*t^2
		FVector Point = LaunchLocation + LaunchVelocity * T + FVector(0.f, 0.f, 0.5f * GravityZ * T * T);
		Points.Add(Point);
	}

	return Points;
}

FVector URageInMageAbilitySystemLibrary::SnapToGround(
	const UObject* WorldContextObject,
	const FVector& Location,
	float TraceDistance)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return Location;

	FHitResult GroundHit;
	const FVector TraceStart = Location + FVector(0.f, 0.f, TraceDistance);
	const FVector TraceEnd = Location - FVector(0.f, 0.f, TraceDistance);

	if (World->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility))
	{
		return GroundHit.ImpactPoint;
	}

	return Location;
}

float URageInMageAbilitySystemLibrary::GetArcFromDistance(
	float Distance, float MaxRange, float MinArc, float MaxArc)
{
	if (MaxRange <= 0.f) return (MinArc + MaxArc) * 0.5f;
	const float Alpha = FMath::Clamp(Distance / MaxRange, 0.f, 1.f);
	return FMath::Lerp(MinArc, MaxArc, Alpha);
}

FLinearColor URageInMageAbilitySystemLibrary::GetClassProgressBarColor(const UObject* WorldContextObject,
	ECharacterClass CharacterClass)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!ClassInfo) return FLinearColor::White;
	const FCharacterClassDefaultInfo& Info = ClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	return Info.ProgressBarColor.GetSpecifiedColor();
}

// ──────────────────────────────────────────
// Decal Utilities
// ──────────────────────────────────────────

void URageInMageAbilitySystemLibrary::SetDecalSize(UDecalComponent* DecalComponent, const FVector& NewSize)
{
	if (DecalComponent)
	{
		DecalComponent->DecalSize = NewSize;
		DecalComponent->MarkRenderStateDirty();
	}
}

// ──────────────────────────────────────────
// InputConfig Lookups
// ──────────────────────────────────────────

bool URageInMageAbilitySystemLibrary::GetInputTagsForAbilityType(
	const FGameplayTag& AbilityTypeTag, const URageInMageConfig* InputConfig,
	FGameplayTag& OutInputTag, FGameplayTag& OutModifierTag)
{
	if (!InputConfig || !AbilityTypeTag.IsValid()) return false;

	for (const FMageInputAction& MageAction : InputConfig->AbilityInputActions)
	{
		if (MageAction.AbilityTypeTag.MatchesTagExact(AbilityTypeTag))
		{
			OutInputTag = MageAction.InputTag;
			OutModifierTag = MageAction.InputModifierTag;
			return true;
		}
	}

	return false;
}

const UInputAction* URageInMageAbilitySystemLibrary::FindInputActionByTags(
	const URageInMageConfig* InputConfig,
	const FGameplayTag& InputTag, const FGameplayTag& ModifierTag)
{
	if (!InputConfig || !InputTag.IsValid()) return nullptr;

	if (ModifierTag.IsValid())
	{
		// Modifier provided: find the entry matching both InputTag AND ModifierTag
		for (const FMageInputAction& MageAction : InputConfig->AbilityInputActions)
		{
			if (MageAction.InputTag.MatchesTagExact(InputTag)
				&& MageAction.InputModifierTag.MatchesTagExact(ModifierTag))
			{
				return MageAction.InputAction;
			}
		}
	}
	else
	{
		// No modifier: find the entry matching InputTag that has NO modifier
		for (const FMageInputAction& MageAction : InputConfig->AbilityInputActions)
		{
			if (MageAction.InputTag.MatchesTagExact(InputTag)
				&& !MageAction.InputModifierTag.IsValid())
			{
				return MageAction.InputAction;
			}
		}
	}

	return nullptr;
}

// ──────────────────────────────────────────
// Key Icon Lookup
// ──────────────────────────────────────────

FKey URageInMageAbilitySystemLibrary::GetKeyForInputTag(
	const FGameplayTag& InputTag, const URageInMageConfig* InputConfig, const UInputMappingContext* IMC)
{
	if (!InputConfig || !IMC || !InputTag.IsValid()) return FKey();

	// Step 1: Find the InputAction mapped to this tag in the InputConfig
	const UInputAction* TargetAction = InputConfig->FindAbilityInputActionForTag(InputTag);
	if (!TargetAction) return FKey();

	// Step 2: Find which key is mapped to that action in the IMC
	const TArray<FEnhancedActionKeyMapping>& Mappings = IMC->GetMappings();
	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		if (Mapping.Action == TargetAction)
		{
			return Mapping.Key;
		}
	}

	return FKey();
}

bool URageInMageAbilitySystemLibrary::GetIconForKey(
	const FKey& InKey, const UDataTable* KeyIconTable, FKeyIconRow& OutRow)
{
	if (!KeyIconTable || !InKey.IsValid()) return false;

	// Row names in the DataTable match FKey names (e.g. "LeftMouseButton", "Gamepad_RightTrigger")
	const FName RowName = InKey.GetFName();
	if (const FKeyIconRow* FoundRow = KeyIconTable->FindRow<FKeyIconRow>(RowName, TEXT("GetIconForKey")))
	{
		OutRow = *FoundRow;
		return true;
	}

	return false;
}

UTexture2D* URageInMageAbilitySystemLibrary::GetIconTextureForInputTag(
	const FGameplayTag& InputTag, const URageInMageConfig* InputConfig,
	const UInputMappingContext* IMC, const UDataTable* KeyIconTable)
{
	const FKey BoundKey = GetKeyForInputTag(InputTag, InputConfig, IMC);
	if (!BoundKey.IsValid()) return nullptr;

	FKeyIconRow IconRow;
	if (GetIconForKey(BoundKey, KeyIconTable, IconRow))
	{
		return IconRow.Icon.LoadSynchronous();
	}

	return nullptr;
}
