#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "PINGPONGGameModeBase.generated.h"

UCLASS()
class PINGPONG_API APINGPONGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY()
    class APingPongPlayerController* Player1 = NULL;
	
    UPROPERTY()
    class APingPongPlayerController* Player2 = NULL;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APlayerStart* Player1Start;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APlayerStart* Player2Start;

	UPROPERTY()
	int32 Player1Score { 0 };

	UPROPERTY()
	int32 Player2Score { 0 };

public:

	UPROPERTY()
	int32 BallPower { 0 };

	APINGPONGGameModeBase();
	
	virtual void BeginPlay() override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void PlayerGoal(int32 PlayerID);

	bool StartGame();
};
