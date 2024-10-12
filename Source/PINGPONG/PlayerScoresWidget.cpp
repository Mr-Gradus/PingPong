#include "PlayerScoresWidget.h"

void UPlayerScoresWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerScoresWidget::SwitchWidget(int32 WindowId)
{
	WidgetSwitcher->SetActiveWidgetIndex(WindowId);
}

void UPlayerScoresWidget::UpdatePlayerScore(int NewValue)
{
	if (Text_PlayerScore)
	{
		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(NewValue));
		auto String { FString::Format(TEXT("Player1 score: {0}") , Args) };
		Text_PlayerScore->SetText(FText::FromString(String));
	}
}

void UPlayerScoresWidget::UpdateBallPower(int NewValue)
{
	if (Text_BallPower)
	{
		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(NewValue));
		auto String { FString::Format(TEXT("Ball power : {0}") , Args) };
		Text_BallPower->SetText(FText::FromString(String));
	}
}

void UPlayerScoresWidget::UpdateEnemyScore(int NewValue)
{
	if (Text_EnemyScore)
	{
		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(NewValue));
		auto String { FString::Format(TEXT("Player2 score: {0}") , Args) };
		Text_EnemyScore->SetText(FText::FromString(String));
	}
}
