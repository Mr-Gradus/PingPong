#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "PlayerScoresWidget.generated.h"


UCLASS()
class PINGPONG_API UPlayerScoresWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_PlayerScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_EnemyScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_BallPower;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;
	
public:
	
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SwitchWidget(int32 ID);
	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerScore(int NewValue);

	UFUNCTION(BlueprintCallable)
	void UpdateBallPower(int NewValue);

	UFUNCTION(BlueprintCallable)
	void UpdateEnemyScore(int NewValue);
};
