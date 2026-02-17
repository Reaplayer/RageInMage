// Copyright Reaplays

#include "UI/Widget/LevelUpMessageWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Engine/Font.h"

void ULevelUpMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULevelUpMessageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsRevealing || bIsSpinning || bIsTransforming)
	{
		CurrentAnimationTime += InDeltaTime;
	}

	if (bIsSpinning)
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
	bIsSpinning = true;
	CurrentLetterIndex = 0;
	CurrentAnimationTime = 0.0f;

	// Set up a timer to reveal letters one by one
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
		// All letters revealed, stop the timer and start the transform phase
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

	// DEBUG: Log what character we're processing
	UE_LOG(LogTemp, Warning, TEXT("Character: '%c' (ASCII: %d)"), CurrentChar, (int32)CurrentChar);

	// Create a new text block for this letter
	UTextBlock* LetterWidget = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	if (LetterWidget)
	{
		// Set initial properties - display as rune
		FString RuneChar = GetRuneForCharacter(CurrentChar);

		// DEBUG: Log what rune we got back
		UE_LOG(LogTemp, Warning, TEXT("Rune returned: %s"), *RuneChar);

		LetterWidget->SetText(FText::FromString(RuneChar));

		// Set font size and color
		FSlateFontInfo FontInfo = LetterWidget->GetFont();
		if (RuneFont && RuneFont->GetCompositeFont())
		{
			// Use the custom rune font if provided
			UE_LOG(LogTemp, Warning, TEXT("Using RuneFont: %s"), *RuneFont->GetName());
			FontInfo.FontObject = RuneFont;
			FontInfo.Size = FontSize;
		}
		else
		{
			// Fallback to the default font
			UE_LOG(LogTemp, Error, TEXT("RuneFont is NULL! Please assign a font in the widget properties!"));
			FontInfo.Size = FontSize;
		}
		LetterWidget->SetFont(FontInfo);
		LetterWidget->SetColorAndOpacity(TextColor);

		// Add to the container with initial spacing
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
	bool bAnyLetterStillSpinning = false;

	// Each letter spins for SpinDuration seconds when it appears
	for (FLetterAnimationData& AnimData : LetterAnimationData)
	{
		if (!AnimData.Widget) continue;

		// Calculate how long this letter has been visible (time since it spawned)
		float LetterAge = CurrentAnimationTime - AnimData.SpawnTime;

		if (LetterAge >= 0.0f && LetterAge <= SpinDuration)
		{
			bAnyLetterStillSpinning = true;

			// Spin animation progress (0 to 1)
			float SpinProgress = LetterAge / SpinDuration;

			// Rotate from 0 to 360 degrees with ease-in-out
			float EasedProgress = FMath::InterpEaseInOut(0.0f, 1.0f, SpinProgress, 2.0f);
			float RotationAngle = EasedProgress * 360.0f;
			
			// Apply an increasing scale size
			float ScaleProgress = FMath::Clamp(LetterAge / SpinDuration, 0.0f, 1.0f);
			float ScaleFactor = FMath::InterpEaseInOut(0.0f, 1.0f, ScaleProgress, 2.0f);

			// Apply rotation and scale
			FWidgetTransform Transform;
			Transform.Angle = RotationAngle;
			Transform.Scale = FVector2D(ScaleFactor, ScaleFactor);
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

	// If no letters are spinning anymore, turn off the spinning flag
	if (!bAnyLetterStillSpinning && LetterAnimationData.Num() > 0)
	{
		bIsSpinning = false;
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

		// At the midpoint of the animation, transform from runes to readable text
		if (Progress >= 0.5f && !AnimData.bHasTransformed)
		{
			// Mark as transformed so we only do this once
			AnimData.bHasTransformed = true;

			// Switch from rune font to text font
			FSlateFontInfo NewFontInfo = AnimData.Widget->GetFont();
			if (TextFont && TextFont->GetCompositeFont())
			{
				// Use the custom text font for readable letters
				UE_LOG(LogTemp, Warning, TEXT("Switching to TextFont: %s"), *TextFont->GetName());
				NewFontInfo.FontObject = TextFont;
				NewFontInfo.Size = FontSize;
			}
			else
			{
				// Fallback to the default font (Roboto)
				UE_LOG(LogTemp, Error, TEXT("TextFont is NULL! Using default font."));
				NewFontInfo.FontObject = nullptr;
				NewFontInfo.TypefaceFontName = FName("Roboto");
				NewFontInfo.Size = FontSize;
			}
			AnimData.Widget->SetFont(NewFontInfo);

			// If the actual character is a number, we need to change the text back to the number
			// (since we mapped it to a letter for the rune font)
			if (AnimData.ActualCharacter >= '0' && AnimData.ActualCharacter <= '9')
			{
				// Change from a letter (A-J) back to a number (0-9)
				AnimData.Widget->SetText(FText::FromString(FString::Chr(AnimData.ActualCharacter)));
				UE_LOG(LogTemp, Warning, TEXT("Converting back to number: %c"), AnimData.ActualCharacter);
			}
			// For letters and other characters, the text stays the same (font change is enough)

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

		// Ensure the final state is set correctly
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
	// Since the Elder Futhark font maps A-Z to runes directly (not using Unicode codepoints),
	// we just return the character as-is and let the font handle the conversion!

	// Convert to uppercase for consistency
	TCHAR UpperChar = FChar::ToUpper(Character);

	// For letters A-Z, just return the letter - the font will convert it to a rune
	if (UpperChar >= 'A' && UpperChar <= 'Z')
	{
		return FString::Chr(UpperChar);
	}

	// For spaces, return a space
	if (UpperChar == ' ')
	{
		return TEXT(" ");
	}

	// For numbers 0-9, map them to letters so the font can display them as runes
	// Since most rune fonts don't have number glyphs, we'll map 0-9 to A-J
	if (UpperChar >= '0' && UpperChar <= '9')
	{
		// Map 0->A, 1->B, 2->C, ... 9->J
		TCHAR MappedChar = 'A' + (UpperChar - '0');
		return FString::Chr(MappedChar);
	}

	// For any other character, return 'A' as a default
	return TEXT("A");
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
	bIsSpinning = false;
	bIsTransforming = false;
	TransformTimer = 0.0f;
}
