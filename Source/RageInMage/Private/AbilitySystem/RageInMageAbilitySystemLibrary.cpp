// Copyright Reaplays


#include "AbilitySystem/RageInMageAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RageInMageAbilitySystemTypes.h"
#include "AbilitySystem/Data/ConditionInfo.h"
#include "Character/RageInMageCharacterBase.h"
#include "Game/RageInMageGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "Game/RageInMageGameState.h"
#include "Player/RageInMagePlayerState.h"
#include "RageInMage/RageInMageLogChannels.h"
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
		return true;
	}

	return false;
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
	

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
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
