// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RageInMageGameplayAbility.generated.h"

/** Shape of the aim indicator displayed while aiming a spell. */
UENUM(BlueprintType)
enum class EAimIndicatorShape : uint8
{
	/** No indicator shown. */
	None,
	/** Circular decal for AoE spells (meteor, fire zone, explosion). Uses AimIndicatorRadius. */
	Circle,
	/** Line/directional for projectile spells (searing flame). Uses AimIndicatorLength/Width. */
	Line,
	/** Box/rectangle for wall spells (fire wall). Uses AimIndicatorLength/Width. */
	Box
};

/**
 * Base gameplay ability for RageInMage.
 * All 72 spells inherit from this (via DamageGameplayAbility, ProjectileSpell, etc.).
 * Provides aim system properties and helpers for the hold-to-aim pattern.
 */
UCLASS()
class RAGEINMAGE_API URageInMageGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URageInMageGameplayAbility();

	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupAbilityTag;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupAbilityTypeTag;

	/** Optional controller input tag override. If empty, auto-mapped from StartupInputTag
	 *  via FRageInMageGameplayTag::InputTagToControllerInputTag at ability grant time. */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupControllerInputTag;

	// ── Aim System ──

	/** If true, this ability uses hold-to-aim: press = enter aim mode, release = cast.
	 *  Set to false for channeled spells that fire immediately on press. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim")
	bool bUseHoldToAim = true;

	/** Shape of the aim indicator. Determines what the Blueprint spawns as a preview. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Prediction")
	EAimIndicatorShape AimIndicatorShape = EAimIndicatorShape::None;

	/** Radius for Circle indicators (AoE spells). World units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Prediction",
		meta = (EditCondition = "AimIndicatorShape == EAimIndicatorShape::Circle", EditConditionHides))
	float AimIndicatorRadius = 0.f;

	/** Length for Line/Box indicators (projectile path or wall length). World units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Prediction",
		meta = (EditCondition = "AimIndicatorShape == EAimIndicatorShape::Line || AimIndicatorShape == EAimIndicatorShape::Box", EditConditionHides))
	float AimIndicatorLength = 0.f;

	/** Width for Line/Box indicators (projectile width or wall thickness). World units. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Prediction",
		meta = (EditCondition = "AimIndicatorShape == EAimIndicatorShape::Line || AimIndicatorShape == EAimIndicatorShape::Box", EditConditionHides))
	float AimIndicatorWidth = 0.f;

	/** How far forward (along the aim direction) to offset the indicator decal from the caster.
	 *  Useful for Line indicators so the decal doesn't overlap the character. 0 = centered on caster. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Prediction")
	float IndicatorForwardOffset = 0.f;

	/** Maximum range of this spell from the caster. 0 = unlimited. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Prediction")
	float MaxSpellRange = 0.f;

	/** Get the current aim world position from the owning player controller. */
	UFUNCTION(BlueprintPure, Category = "Aim")
	FVector GetAimWorldPosition() const;
};
