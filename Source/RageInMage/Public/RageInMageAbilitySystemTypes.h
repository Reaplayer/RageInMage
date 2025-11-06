#pragma once

#include "GameplayEffectTypes.h"
#include "RageInMageAbilitySystemTypes.generated.h"

USTRUCT(BlueprintType)
struct FRageInMageGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	// Constructor
	FRageInMageGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}
	
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsVulnerableHit() const { return bIsVulnerableHit; }
	bool IsResistantHit() const { return bIsResistantHit; }

	void SetIsCriticalHit(bool bIsInCriticalHit) { bIsCriticalHit = bIsInCriticalHit; }
	void SetIsVulnerableHit(bool bIsInVulnerableHit) { bIsVulnerableHit = bIsInVulnerableHit; }
	void SetIsResistantHit(bool bIsInResistantHit) { bIsResistantHit = bIsInResistantHit; }

	/* Returns the actual struct used for Serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FRageInMageGameplayEffectContext* Duplicate() const override
	{
		FRageInMageGameplayEffectContext* NewContext = new FRageInMageGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of Hit Result
			NewContext->AddHitResult(*GetHitResult());
		}
		return NewContext;
	}

	/* Custom Serialization, subclasses must override this! */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsCriticalHit = false;
	UPROPERTY()
	bool bIsVulnerableHit = false;
	UPROPERTY()
	bool bIsResistantHit = false;
};

// Required template specialization for proper memory management
template<>
struct TStructOpsTypeTraits<FRageInMageGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FRageInMageGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
