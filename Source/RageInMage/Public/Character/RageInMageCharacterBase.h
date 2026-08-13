// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/RageInMageAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "RageInMageCharacterBase.generated.h"


class UGameplayEffect;
class UAttributeSet;
class UGameplayAbility;
class UAnimMontage;
class UNiagaraComponent;

/** Struggle (mash-to-escape) progress, 0-1. For driving a break-free UI bar. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStruggleProgressChanged, float, ProgressPercent);

UCLASS(Abstract)
class RAGEINMAGE_API ARageInMageCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// The actual delegate instance. "BlueprintAssignable" allows you to bind to it in BP.
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDeathSignature OnDeathDelegate;
	
	ARageInMageCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	URageInMageAttributeSet* GetRageInMageAttributeSet() const;

	/** Accessors for combat socket components/names (used by abilities to attach VFX). */
	USkeletalMeshComponent* GetWeapon() const { return Weapon; }
	FName GetWeaponTipSocketName() const { return WeaponTipSocketName; }
	FName GetLeftHandSocketName() const { return LeftHandSocketName; }
	FName GetRightHandSocketName() const { return RightHandSocketName; }
	FName GetTailSocketName() const { return TailSocketName; }

	/* Combat Interface */
	virtual UAnimMontage* GetHitReactionMontage_Implementation() override;
	virtual void Die() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetRandomAttackMontage_Implementation(bool bIsRanged, bool bIsSummon) override;
	virtual int32 GetSummonCount_Implementation() override;
	virtual void SetSummonCount_Implementation(int32 NewSummonCount) override;
	virtual int32 GetMaxSummonCount_Implementation() override;
	virtual void SetMaxSummonCount_Implementation(int32 NewMaxSummonCount) override;
	virtual void RegisterSpawnedMinion_Implementation(AActor* Minion) override;
	/* End Combat Interface */

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	virtual FOnDeathSignature& GetOnDeathDelegate() override { return OnDeathDelegate; }

	/**
	 * Launches this character with a knockback impulse, scaled down by this character's own Poise
	 * (gear that grants Poise makes knockback "not so effective" on the wearer). All knockback sources
	 * (abilities, projectiles, wave drag, etc.) should route through this instead of calling
	 * LaunchCharacter directly, so collision-transfer/bounce in NotifyHit applies uniformly everywhere.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Knockback")
	void ApplyKnockbackImpulse(FVector Impulse, bool bXYOverride = true, bool bZOverride = true);

	/** Current BaseWalkSpeed (before the MovementSpeed attribute multiplier). Lets abilities (e.g. a
	 *  dash that temporarily boosts walk speed) save off the value to restore later. */
	UFUNCTION(BlueprintPure, Category = "Combat|MovementSpeed")
	float GetBaseWalkSpeed() const { return BaseWalkSpeed; }

	/** Sets BaseWalkSpeed and immediately recomputes MaxWalkSpeed from it. Lets abilities drive
	 *  movement speed directly (e.g. a dash) while still going through the MovementSpeed multiplier. */
	UFUNCTION(BlueprintCallable, Category = "Combat|MovementSpeed")
	void SetBaseWalkSpeed(float NewBaseWalkSpeed);

	UFUNCTION()
	void OnMinionDeath(AActor* DeadActor);

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UNiagaraSystem* BloodEffect;
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* DeathSound;
	
	/* Summons */
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 SummonCount = 0;
	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxSummonCount = 5;
	/* End Summons */
	
	/** True if enough time has passed since the last hit reaction ended (or none has happened yet). */
	bool CanTriggerHitReaction() const;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Below this horizontal speed, a blocking hit is treated as incidental movement (e.g. AI pathing bumping into someone), not an active knockback — no transfer/bounce occurs. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Knockback")
	float MinKnockbackTransferSpeed = 300.f;

	/** Base resistance (in cm/s of incoming speed) a character presents to being shoved by another character's body, before the target's own Poise is added. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Knockback")
	float BaseShoveResistance = 400.f;

	/** Additional shove resistance contributed per point of the target's Poise attribute. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Knockback")
	float PoiseResistancePerPoint = 5.f;

	/** Fraction of the mover's momentum that transfers into a target that CAN be moved (0-1). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Knockback")
	float KnockbackTransferRatio = 0.6f;

	/** How much of its own velocity the mover loses on impact with a movable target, scaled by how much resistance it had to overcome (0-1). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Knockback")
	float KnockbackAbsorptionRatio = 0.5f;

	/** Velocity retained (as a fraction) when bouncing off a target/object that couldn't be moved at all. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Knockback")
	float KnockbackBounceRestitution = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

	bool bDead = false;

	// Transient on purpose. Both are established at runtime — the player's come from the PlayerState in
	// InitPlayerAbilityActorInfo(), the enemy's from CreateDefaultSubobject in its constructor — so neither
	// should ever be written to disk. Without Transient the pointer serializes into any child Blueprint's
	// CDO and can outlive the code that set it: that is exactly how BP_RageInMagePlayerCharacter ended up
	// carrying a phantom pawn-owned ASC with zero attribute sets long after the constructor stopped
	// creating one. See the "STRAY PAWN-OWNED ASC" section in .claude/memory.md (2026-07-24).
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient)
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	virtual void InitPlayerAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultResistanceAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultItemSpecificAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	/* Condition VFX */

	/** Niagara system to spawn on the character when burning. Should have a 'StackCount' user param. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|Burning")
	TObjectPtr<UNiagaraSystem> BurningVFXSystem;

	/** Binds the ignite stack count delegate from the ASC to drive burning VFX. Call after ASC is initialized. */
	void BindIgniteStackDelegate();

	/* Heat Glow VFX — subtle emissive tint driven by Heat attribute */

	/** Emissive intensity at heat near zero. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	float HeatGlowMinIntensity = 0.f;

	/** Emissive intensity at max heat (±120). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	float HeatGlowMaxIntensity = 2.0f;

	/** Glow color for positive heat (fire). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	FLinearColor HeatGlowWarmColor = FLinearColor(1.0f, 0.3f, 0.05f, 1.0f);

	/** Glow color for negative heat (cold). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	FLinearColor HeatGlowColdColor = FLinearColor(0.1f, 0.4f, 1.0f, 1.0f);

	/** Mesh material slot indices to apply the glow to. Eyes (slot 2) excluded by default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|HeatGlow")
	TArray<int32> HeatGlowMaterialSlots = {0, 1, 3, 4, 5};

	/** Binds the Heat attribute change delegate. Call after ASC is initialized. */
	void BindHeatGlowDelegate();

	/* Crowd control — every incapacitating condition locks movement here, and blocks ability activation
	 * via URageInMageGameplayAbility's ActivationBlockedTags. Driven by tag SETS rather than
	 * Condition.Stunned alone, so Frozen/Petrified/Grappled/... each work on their own and, crucially,
	 * CHAINED CCs overlap safely (stun -> petrify -> freeze stays locked until the last one clears). */

	/** Conditions that stop BOTH movement and actions ("unable to Move or make any Action").
	 *  Left empty it lazily fills with the full-incapacitation set in BindCrowdControlDelegates. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|CrowdControl")
	FGameplayTagContainer IncapacitationTags;

	/** Conditions that stop movement ONLY - the character can still act (e.g. Rooted). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|CrowdControl")
	FGameplayTagContainer MovementOnlyBlockTags;

	/** Every tag that blocks movement: IncapacitationTags + MovementOnlyBlockTags. */
	FGameplayTagContainer GetMovementBlockingTags() const;

	/** Binds all movement-blocking condition tags to lock/unlock movement. Call after ASC is initialized. */
	void BindCrowdControlDelegates();

	/* Struggle — conditions flagged bCanStruggleFree in ConditionInfo (Petrified, Grappled) can be
	 * escaped early by mashing an input rather than waiting the duration out. */

	/** Call from input on every mash. Safe to call on the owning client - it routes to the server,
	 *  which owns the progress and the actual break-out. No-op when not struggling. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Struggle")
	void AddStruggleProgress();

	/** True while an escapable condition is active on this character. */
	UFUNCTION(BlueprintPure, Category = "Combat|Struggle")
	bool IsStruggling() const { return ActiveStruggleTag.IsValid(); }

	/** Current escape progress as 0-1, for a mash/struggle UI bar. */
	UFUNCTION(BlueprintPure, Category = "Combat|Struggle")
	float GetStruggleProgressPercent() const;

	/** Broadcast (server + owning client) whenever struggle progress changes. 0-1. */
	UPROPERTY(BlueprintAssignable, Category = "Combat|Struggle")
	FOnStruggleProgressChanged OnStruggleProgressChanged;

	/* Hit Reaction Grace — deliberately separate from Condition.Stunned. GA_HitReaction already
	 * blocks retriggering while one is still playing (InstancedPerActor, bRetriggerInstancedAbility
	 * false), but nothing stopped a new one starting the instant the last one ended — multi-hit
	 * abilities (e.g. Lightning Flash's rods) could chain flinches with no recovery window, which
	 * reads as a stun-lock even though no CC is actually applied. */

	/** Minimum time after a hit reaction ends before another can start. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|HitReaction")
	float HitReactionGraceSeconds = 0.3f;

	/** Binds the Effects.HitReaction tag delegate to track when reactions end. Call after ASC is initialized. */
	void BindHitReactionGraceDelegate();

	/* Movement Speed — MovementSpeed attribute is a multiplier (1.0 = unchanged) applied to BaseWalkSpeed,
	 * so any GE that modifies it (slows, haste) takes effect live instead of only at BeginPlay. */

	/** Base walk speed before the MovementSpeed attribute multiplier is applied. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|MovementSpeed")
	float BaseWalkSpeed = 600.f;

	/** Recomputes CharacterMovementComponent's MaxWalkSpeed from BaseWalkSpeed * MovementSpeed. */
	UFUNCTION(BlueprintCallable, Category = "Combat|MovementSpeed")
	void UpdateMovementSpeed();

	/** Binds the MovementSpeed attribute change delegate to live-update walk speed. Call after ASC is initialized. */
	void BindMovementSpeedDelegate();

	/* Dissolve Effects */
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartMeshDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> MeshDissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	/* Burning VFX */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BurningVFXComponent;

	UFUNCTION()
	void OnIgniteStackCountChanged(int32 NewStackCount);

	/* Heat Glow */
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> HeatGlowDMIs;
	bool bHeatGlowDMIsCreated = false;
	void CreateHeatGlowDMIs();

	/* Crowd control */
	void CrowdControlTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	/** Movement mode captured on the FIRST lock only, so overlapping CCs can't save an already-disabled mode. */
	TEnumAsByte<EMovementMode> MovementModeBeforeCC = MOVE_Walking;
	bool bMovementBlockedByCC = false;

	/* Struggle (server-authoritative) */

	/** Server owns progress and the break-out; clients just report mashes. */
	UFUNCTION(Server, Reliable)
	void ServerAddStruggleProgress();

	/** Starts a struggle for CondTag if its ConditionInfo row allows it. */
	void BeginStruggle(const FGameplayTag& CondTag);
	/** Clears struggle state (condition expired, broken out of, or replaced). */
	void EndStruggle();
	/** Bleeds StruggleDecayPerSecond so escaping needs fast mashing, not patient mashing. */
	void TickStruggleDecay();
	/** Removes the struggled-out-of condition and clears state. */
	void BreakFreeFromStruggle();

	/** The escapable condition currently being struggled against; invalid when not struggling. */
	FGameplayTag ActiveStruggleTag;
	float StruggleProgress = 0.f;
	/** Cached from the ConditionInfo row when the struggle begins. */
	float StruggleRequiredProgress = 100.f;
	float StruggleProgressPerMash = 10.f;
	float StruggleDecayPerSecond = 5.f;
	FTimerHandle StruggleDecayTimerHandle;

	/* Hit Reaction Grace */
	void HitReactionGraceTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	float LastHitReactionEndTime = -1000.f;
	void OnHeatAttributeChanged(const FOnAttributeChangeData& Data);

	/* Movement Speed */
	void OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data);

	/* Movement Speed debug — temporary instrumentation for tracking down the "MovementSpeed=0" bug.
	 * Ticking is disabled project-wide (see PrimaryActorTick.bCanEverTick = false in the constructor),
	 * so this uses a repeating timer instead of Tick() to avoid changing that perf characteristic. */
	FTimerHandle MovementSpeedDebugTimerHandle;
	void LogMovementSpeedDebug();

	/* Knockback collision transfer — de-dupes repeated NotifyHit calls against the same actor within one resolution window (sliding/multi-sweep can fire NotifyHit several times per frame). */
	TWeakObjectPtr<AActor> LastKnockbackHitActor;
	float LastKnockbackHitTime = -1.f;
};
