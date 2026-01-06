// Copyright Reaplays

#include "UI/Widget/LevelUpMessageWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void ULevelUpMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULevelUpMessageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsRevealing || bIsTransforming)
	{
		CurrentAnimationTime += InDeltaTime;
	}

	if (bIsRevealing)
	{
		UpdateLetterSpinAnimation(InDeltaTime);
	}

	if (bIsTransforming)
	{
		UpdateTransformAnimation(InDeltaTime);
	}
}

void ULevelUpMessageWidget::PlayLevelUpAnimation(const FText& Message, UHorizontalBox* TargetContainer)
{
	PlayMultiLineLevelUpAnimation(Message, TargetContainer);
}

void ULevelUpMessageWidget::PlayMultiLineLevelUpAnimation(const FText& Line1, UHorizontalBox* Container1, const FText& Line2, UHorizontalBox* Container2, const FText& Line3, UHorizontalBox* Container3)
{
	CleanupAnimation();

	// Gather all lines and containers that are valid
	LinesToDisplay.Empty();
	TargetContainers.Empty();

	if (!Line1.IsEmpty() && Container1)
	{
		LinesToDisplay.Add(Line1.ToString());
		TargetContainers.Add(Container1);
	}
	if (!Line2.IsEmpty() && Container2)
	{
		LinesToDisplay.Add(Line2.ToString());
		TargetContainers.Add(Container2);
	}
	if (!Line3.IsEmpty() && Container3)
	{
		LinesToDisplay.Add(Line3.ToString());
		TargetContainers.Add(Container3);
	}

	if (LinesToDisplay.Num() == 0 || TargetContainers.Num() == 0)
	{
		return;
	}

	// Start the reveal phase
	bIsRevealing = true;
	CurrentLetterIndex = 0;
	CurrentAnimationTime = 0.0f;

	// Set up timer to reveal letters one by one
	GetWorld()->GetTimerManager().SetTimer(
		LetterRevealTimerHandle,
		this,
		&ULevelUpMessageWidget::RevealNextLetter,
		LetterRevealDelay,
		true
	);
}

void ULevelUpMessageWidget::RevealNextLetter()
{
	// Calculate total letters across all lines
	int32 TotalLetters = 0;
	for (const FString& Line : LinesToDisplay)
	{
		TotalLetters += Line.Len();
	}

	if (CurrentLetterIndex >= TotalLetters)
	{
		// All letters revealed, stop the timer and start transform phase
		GetWorld()->GetTimerManager().ClearTimer(LetterRevealTimerHandle);
		bIsRevealing = false;

		// Wait for the last letter's spin to complete, then transform
		GetWorld()->GetTimerManager().SetTimer(
			TransformTimerHandle,
			this,
			&ULevelUpMessageWidget::StartTransformPhase,
			SpinDuration,
			false
		);
		return;
	}

	// Figure out which line and which character within that line we're on
	int32 CharsSoFar = 0;
	int32 CurrentLineIndex = 0;
	int32 CharIndexInLine = 0;

	for (int32 LineIdx = 0; LineIdx < LinesToDisplay.Num(); LineIdx++)
	{
		if (CurrentLetterIndex < CharsSoFar + LinesToDisplay[LineIdx].Len())
		{
			CurrentLineIndex = LineIdx;
			CharIndexInLine = CurrentLetterIndex - CharsSoFar;
			break;
		}
		CharsSoFar += LinesToDisplay[LineIdx].Len();
	}

	// Get the appropriate container for this line from our stored containers
	if (CurrentLineIndex < 0 || CurrentLineIndex >= TargetContainers.Num())
	{
		CurrentLetterIndex++;
		return;
	}

	UHorizontalBox* TargetContainer = TargetContainers[CurrentLineIndex];
	if (!TargetContainer)
	{
		CurrentLetterIndex++;
		return;
	}

	// Get the character to display
	TCHAR CurrentChar = LinesToDisplay[CurrentLineIndex][CharIndexInLine];

	// Create a new text block for this letter
	UTextBlock* LetterWidget = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	if (LetterWidget)
	{
		// Set initial properties - display as rune
		FString RuneChar = GetRuneForCharacter(CurrentChar);
		LetterWidget->SetText(FText::FromString(RuneChar));

		// Set font size and color
		FSlateFontInfo FontInfo = LetterWidget->GetFont();
		FontInfo.Size = FontSize;
		LetterWidget->SetFont(FontInfo);
		LetterWidget->SetColorAndOpacity(TextColor);

		// Add to container with initial spacing
		UHorizontalBoxSlot* BoxSlot = TargetContainer->AddChildToHorizontalBox(LetterWidget);
		if (BoxSlot)
		{
			BoxSlot->SetPadding(FMargin(InitialLetterSpacing / 2.0f, 0.0f, InitialLetterSpacing / 2.0f, 0.0f));
			BoxSlot->SetHorizontalAlignment(HAlign_Center);
			BoxSlot->SetVerticalAlignment(VAlign_Center);
		}

		// Store the widget, spawn time, actual character, and parent container
		FLetterAnimationData AnimData;
		AnimData.Widget = LetterWidget;
		AnimData.SpawnTime = CurrentAnimationTime;
		AnimData.ActualCharacter = CurrentChar;
		AnimData.ParentContainer = TargetContainer;
		LetterAnimationData.Add(AnimData);
	}

	CurrentLetterIndex++;
}

void ULevelUpMessageWidget::UpdateLetterSpinAnimation(float DeltaTime)
{
	// Each letter spins for SpinDuration seconds when it appears
	for (FLetterAnimationData& AnimData : LetterAnimationData)
	{
		if (!AnimData.Widget) continue;

		// Calculate how long this letter has been visible (time since it spawned)
		float LetterAge = CurrentAnimationTime - AnimData.SpawnTime;

		if (LetterAge >= 0.0f && LetterAge <= SpinDuration)
		{
			// Spin animation progress (0 to 1)
			float SpinProgress = LetterAge / SpinDuration;

			// Rotate from 0 to 360 degrees with ease-in-out
			float EasedProgress = FMath::InterpEaseInOut(0.0f, 1.0f, SpinProgress, 2.0f);
			float RotationAngle = EasedProgress * 360.0f;

			// Apply rotation
			FWidgetTransform Transform;
			Transform.Angle = RotationAngle;
			AnimData.Widget->SetRenderTransform(Transform);
		}
		else if (LetterAge > SpinDuration)
		{
			// Spin complete, ensure it's at 0 rotation
			FWidgetTransform Transform;
			Transform.Angle = 0.0f;
			AnimData.Widget->SetRenderTransform(Transform);
		}
	}
}

void ULevelUpMessageWidget::StartTransformPhase()
{
	bIsTransforming = true;
	TransformTimer = 0.0f;
}

void ULevelUpMessageWidget::UpdateTransformAnimation(float DeltaTime)
{
	TransformTimer += DeltaTime;
	float Progress = FMath::Clamp(TransformTimer / SlamDuration, 0.0f, 1.0f);

	// Ease-in effect for the "slam" - starts slow, ends fast
	float EasedProgress = FMath::InterpEaseIn(0.0f, 1.0f, Progress, 2.5f);

	// Update letter spacing
	float CurrentSpacing = FMath::Lerp(InitialLetterSpacing, FinalLetterSpacing, EasedProgress);

	// Update all letter widgets
	for (FLetterAnimationData& AnimData : LetterAnimationData)
	{
		if (!AnimData.Widget) continue;

		// Update spacing
		if (UHorizontalBoxSlot* BoxSlot = Cast<UHorizontalBoxSlot>(AnimData.Widget->Slot))
		{
			BoxSlot->SetPadding(FMargin(CurrentSpacing / 2.0f, 0.0f, CurrentSpacing / 2.0f, 0.0f));
		}

		// At the midpoint of the animation, change from runes to actual letters
		if (Progress >= 0.5f && AnimData.Widget->GetText().ToString() != FString::Chr(AnimData.ActualCharacter))
		{
			AnimData.Widget->SetText(FText::FromString(FString::Chr(AnimData.ActualCharacter)));

			// Optional: Add a scale "pop" effect when transforming
			FWidgetTransform Transform = AnimData.Widget->GetRenderTransform();
			Transform.Scale = FVector2D(1.2f, 1.2f);
			AnimData.Widget->SetRenderTransform(Transform);
		}

		// Reset scale back to normal
		if (Progress > 0.5f && Progress < 0.8f)
		{
			float ScaleProgress = (Progress - 0.5f) / 0.3f;
			float CurrentScale = FMath::Lerp(1.2f, 1.0f, ScaleProgress);
			FWidgetTransform Transform = AnimData.Widget->GetRenderTransform();
			Transform.Scale = FVector2D(CurrentScale, CurrentScale);
			AnimData.Widget->SetRenderTransform(Transform);
		}
	}

	// Animation complete
	if (Progress >= 1.0f)
	{
		bIsTransforming = false;

		// Ensure final state is set correctly
		for (FLetterAnimationData& AnimData : LetterAnimationData)
		{
			if (!AnimData.Widget) continue;

			FWidgetTransform Transform = AnimData.Widget->GetRenderTransform();
			Transform.Scale = FVector2D(1.0f, 1.0f);
			Transform.Angle = 0.0f;
			AnimData.Widget->SetRenderTransform(Transform);
		}
	}
}

FString ULevelUpMessageWidget::GetRuneForCharacter(TCHAR Character) const
{
	// Elder Futhark Runes (Unicode U+16A0 to U+16F8)
	// We'll create a simple mapping system

	// Convert to uppercase for consistency
	TCHAR UpperChar = FChar::ToUpper(Character);

	// Map A-Z to runes
	// Using Elder Futhark rune order: ᚠ ᚢ ᚦ ᚨ ᚱ ᚲ ᚷ ᚹ ᚺ ᚾ ᛁ ᛃ ᛇ ᛈ ᛉ ᛊ ᛏ ᛒ ᛖ ᛗ ᛚ ᛜ ᛞ ᛟ
	static const TArray<FString> RuneMapping = {
		TEXT("\u16A0"), // A -> ᚠ (Fehu)
		TEXT("\u16A2"), // B -> ᚢ (Uruz)
		TEXT("\u16A6"), // C -> ᚦ (Thurisaz)
		TEXT("\u16A8"), // D -> ᚨ (Ansuz)
		TEXT("\u16B1"), // E -> ᚱ (Raidho)
		TEXT("\u16B2"), // F -> ᚲ (Kenaz)
		TEXT("\u16B7"), // G -> ᚷ (Gebo)
		TEXT("\u16B9"), // H -> ᚹ (Wunjo)
		TEXT("\u16BA"), // I -> ᚺ (Hagalaz)
		TEXT("\u16BE"), // J -> ᚾ (Nauthiz)
		TEXT("\u16C1"), // K -> ᛁ (Isa)
		TEXT("\u16C3"), // L -> ᛃ (Jera)
		TEXT("\u16C7"), // M -> ᛇ (Eihwaz)
		TEXT("\u16C8"), // N -> ᛈ (Perthro)
		TEXT("\u16C9"), // O -> ᛉ (Algiz)
		TEXT("\u16CA"), // P -> ᛊ (Sowilo)
		TEXT("\u16CF"), // Q -> ᛏ (Tiwaz)
		TEXT("\u16D2"), // R -> ᛒ (Berkano)
		TEXT("\u16D6"), // S -> ᛖ (Ehwaz)
		TEXT("\u16D7"), // T -> ᛗ (Mannaz)
		TEXT("\u16DA"), // U -> ᛚ (Laguz)
		TEXT("\u16DC"), // V -> ᛜ (Ingwaz)
		TEXT("\u16DE"), // W -> ᛞ (Dagaz)
		TEXT("\u16DF"), // X -> ᛟ (Othala)
		TEXT("\u16A0"), // Y -> ᚠ (wrap around)
		TEXT("\u16A2")  // Z -> ᚢ (wrap around)
	};

	// Check if it's a letter A-Z
	if (UpperChar >= 'A' && UpperChar <= 'Z')
	{
		int32 Index = UpperChar - 'A';
		return RuneMapping[Index];
	}

	// For spaces, return a special character or space
	if (UpperChar == ' ')
	{
		return TEXT(" ");
	}

	// For numbers and special characters, use geometric runes
	if (UpperChar >= '0' && UpperChar <= '9')
	{
		// Use some nice runes for numbers
		static const TArray<FString> NumberRunes = {
			TEXT("\u16A0"), TEXT("\u16A2"), TEXT("\u16A6"), TEXT("\u16A8"), TEXT("\u16B1"),
			TEXT("\u16B2"), TEXT("\u16B7"), TEXT("\u16B9"), TEXT("\u16BA"), TEXT("\u16BE")
		};
		return NumberRunes[UpperChar - '0'];
	}

	// Default: return a generic rune
	return TEXT("\u16A0");
}

void ULevelUpMessageWidget::StopLevelUpAnimation()
{
	CleanupAnimation();
	SetVisibility(ESlateVisibility::Hidden);
}

void ULevelUpMessageWidget::CleanupAnimation()
{
	// Clear timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LetterRevealTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(TransformTimerHandle);
	}

	// Remove all letter widgets from all target containers
	for (UHorizontalBox* Container : TargetContainers)
	{
		if (Container)
		{
			Container->ClearChildren();
		}
	}

	LetterAnimationData.Empty();
	LinesToDisplay.Empty();
	TargetContainers.Empty();
	CurrentLetterIndex = 0;
	CurrentAnimationTime = 0.0f;
	bIsRevealing = false;
	bIsTransforming = false;
	TransformTimer = 0.0f;
}
