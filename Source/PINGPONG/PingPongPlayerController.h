#pragma once

#include "CoreMinimal.h"
#include "PingPongGate.h"
#include "PingPongBall.h"
#include "PlayerScoresWidget.h"
#include "GameFramework/PlayerController.h"
#include "PingPongPlayerController.generated.h"

class APingPongPlatform;

UENUM()
enum class PlayerWindowId
{
	WaitForAnotherPlayers = 0,
	Game = 1
};

UCLASS()
class PINGPONG_API APingPongPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	FTransform StartTransform;

	UPROPERTY(Replicated)
	int32 PlayerID { 0 };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class APingPongPlatform> PlatformClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	APingPongPlatform* Platform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	APingPongGate* Gate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	APingPongBall* Ball;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UPlayerScoresWidget* Widget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	UFUNCTION()
	void MoveRight(float AxisValue);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PlatformMoveRight(float AxisValue);

public:

	APingPongPlayerController();
	
    UFUNCTION()
    void SetStartTransform(FTransform NewStartTransform);
		
    UFUNCTION(Server, Reliable, WithValidation)
    void SpawnPlatform(TSubclassOf<class APingPongPlatform> PlatfromClass);
	
    virtual void SetupInputComponent() override;
	
	UFUNCTION()
	FORCEINLINE int32 GetPlayerID() const { return PlayerID; }

	UFUNCTION(Client, Reliable)
	void UpdateWidgetPlayerScore(int32 Score);

	UFUNCTION(Client, Reliable)
	void UpdateWidgetEnemyScore(int32 Score);

	UFUNCTION(Client, Reliable)
	void UpdateWidgetBallPower(int32 Power);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_InitializeHUD();

	UFUNCTION(Client, Reliable)
	void Client_SetHUDWindow(PlayerWindowId windowId);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Initialize(int32 NewPlayerID, APingPongGate* NewGate);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetPlatformSkin(FSoftObjectPath SkinMaterialAsset);

	UFUNCTION(BlueprintCallable)
	void SetBallSkin(FSoftObjectPath SkinMaterialAsset);

	UFUNCTION(BlueprintCallable)
	void SetGateSkin(FSoftObjectPath SkinMaterialAsset);

	UFUNCTION(Server, Reliable)
	void Server_SetPlatformSkin(FSoftObjectPath SkinMaterialAsset);

	UFUNCTION(Server, Reliable)
	void Server_SetBallSkin(FSoftObjectPath SkinMaterialAsset);

	UFUNCTION(Server, Reliable)
	void Server_SetGateSkin(FSoftObjectPath SkinMaterialAsset);
};
