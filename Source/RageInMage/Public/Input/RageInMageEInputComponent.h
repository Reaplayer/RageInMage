// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "RageInMageConfig.h"
#include "RageInMageEInputComponent.generated.h"

/**
 *
 */
UCLASS()
class RAGEINMAGE_API URageInMageEInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const URageInMageConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void URageInMageEInputComponent::BindAbilityActions(const URageInMageConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FMageInputAction Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
				// Chord-triggered actions (controller modifier + button, e.g. LB+RT for the
				// Ultimate) fire Canceled — not Completed — when the chord breaks on release.
				// Without this, hold-to-aim spells bound to a chord activate but never receive
				// their release, so they aim forever and never fire. Plain Pressed triggers
				// don't emit Canceled, so this is harmless for M&K / non-chorded bindings.
				BindAction(Action.InputAction, ETriggerEvent::Canceled, Object, ReleasedFunc, Action.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
