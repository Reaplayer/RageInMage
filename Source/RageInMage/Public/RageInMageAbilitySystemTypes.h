#pragma once

#include "GameplayEffectTypes.h"
#include "RageInMageAbilitySystemTypes.generated.h"

USTRUCT(BlueprintType)
struct FRageInMageGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsVulnerableHit() const { return bIsVulnerableHit; }

	void SetIsCriticalHit(bool bIsInCriticalHit) { bIsCriticalHit = bIsInCriticalHit; }
	void SetIsVulnerableHit(bool bIsInVulnerableHit) { bIsVulnerableHit = bIsInVulnerableHit; }

	/* Returns the actual struct used for Serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	virtual FRageInMageGameplayEffectContext* Duplicate() const
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
};

template<>
struct TStructOpsTypeTraits<FRageInMageGameplayEffectContext> : TStructOpsTypeTraitsBase2<FRageInMageGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
