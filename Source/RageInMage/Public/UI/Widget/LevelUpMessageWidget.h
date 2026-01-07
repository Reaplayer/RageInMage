// Copyright Reaplays

#pragma once

#include "CoreMinimal.h"
#include "MageUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpMessageWidget.generated.h"

class UTextBlock;
class UHorizontalBox;

/**
 * Widget that displays an animated level up message
 * Letters appear one by one as spinning runes, then transform into readable text
 */
UCLASS()
class RAGEINMAGE_API ULevelUpMessageWidget : public UMageUserWidget
{
	GENERATED_BODY()

public:
	// Call this to start the level up animation with a custom message in a specific container
	UFUNCTION(BlueprintCallable, Category = "Level Up")
	void PlayLevelUpAnimation(const FText& Message, UHorizontalBox* TargetContainer);

	// Call this to start the level up animation with multiple lines in different containers
	UFUNCTION(BlueprintCallable, Category = "Level Up")
	void PlayMultiLineLevelUpAnimation(const FText& Line1, UHorizontalBox* Container1, const FText& Line2 = FText(), UHorizontalBox* Container2 = nullptr, const FText& Line3 = FText(), UHorizontalBox* Container3 = nullptr);

	// Stop and hide the widget
	UFUNCTION(BlueprintCallable, Category = "Level Up")
	void StopLevelUpAnimation();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Time delay between each letter appearing (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float LetterRevealDelay = 0.15f;

	// Duration of each letter's spin animation (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float SpinDuration = 0.5f;

	// Initial spacing between letters (rune phase)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float InitialLetterSpacing = 50.0f;

	// Final spacing between letters (text phase)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float FinalLetterSpacing = 5.0f;

	// Duration of the "slam together" animation (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float SlamDuration = 0.4f;

	// Font to use for runes at the start (must support Elder Futhark Unicode characters)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UFont> RuneFont;

	// Font to use for readable text at the end (after transformation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UFont> TextFont;

	// Font size for the letters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	int32 FontSize = 48;

	// Color of the text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FLinearColor TextColor = FLinearColor::Yellow;

private:
	// Structure to track each letter's animation state
	struct FLetterAnimationData
	{
		UTextBlock* Widget = nullptr;
		float SpawnTime = 0.0f;
		TCHAR ActualCharacter = ' ';
		UHorizontalBox* ParentContainer = nullptr;
		bool bHasTransformed = false; // Track if we've already switched from rune to text font
	};

	// Internal state tracking
	TArray<FString> LinesToDisplay;
	TArray<UHorizontalBox*> TargetContainers;
	TArray<FLetterAnimationData> LetterAnimationData;
	int32 CurrentLetterIndex = 0;
	float CurrentAnimationTime = 0.0f;
	bool bIsRevealing = false;
	bool bIsSpinning = false;
	bool bIsTransforming = false;
	float TransformTimer = 0.0f;

	// Timer handles for managing animation phases
	FTimerHandle LetterRevealTimerHandle;
	FTimerHandle TransformTimerHandle;

	// Functions for animation phases
	void RevealNextLetter();
	void StartTransformPhase();
	void UpdateLetterSpinAnimation(float DeltaTime);
	void UpdateTransformAnimation(float DeltaTime);

	// Helper function to map letters to runes
	FString GetRuneForCharacter(TCHAR Character) const;

	// Clean up for restarting animation
	void CleanupAnimation();
};
