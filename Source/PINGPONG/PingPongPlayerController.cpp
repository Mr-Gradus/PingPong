#include "PingPongPlayerController.h"
#include "PingPongPlatform.h"
#include "Net/UnrealNetwork.h"

APingPongPlayerController::APingPongPlayerController()
{
	SetReplicates(true);
}

void APingPongPlayerController::MoveRight(float AxisValue)
{
	if(AxisValue != 0)
	{
		UE_LOG(LogTemp, Warning,TEXT("APingPongPlayerController::MoveRight"));
	}
	
	Server_PlatformMoveRight(AxisValue);
}

bool APingPongPlayerController::Server_PlatformMoveRight_Validate(float AxisValue)
{
	return true;
}

void APingPongPlayerController::Server_PlatformMoveRight_Implementation(float AxisValue)
{
	if(Platform)
    {
		if(AxisValue != 0)
		{
			UE_LOG(LogTemp, Warning,
			TEXT("APingPongPlayerController::Server_PlatformMoveRight_Implementation"));
		}
		
		Platform->Server_MoveRight(AxisValue);
    }
	else
	{
		UE_LOG(LogTemp, Error,
		TEXT("APingPongPlayerController::Server_PlatformMoveRight_Implementation: HAS NO PLATFORM!"));
	}
}

void APingPongPlayerController::SetStartTransform(FTransform NewStartTransform)
{
	StartTransform = NewStartTransform;
}

bool APingPongPlayerController::SpawnPlatform_Validate(TSubclassOf<APingPongPlatform> InPlatformClass)
{
	return InPlatformClass != nullptr;
}

void APingPongPlayerController::SpawnPlatform_Implementation(TSubclassOf<APingPongPlatform> InPlatformClass)
{
	Platform = GetWorld()->SpawnActor<APingPongPlatform>(InPlatformClass);
	
    if(Platform)
    {
	    Platform->SetActorLocation(StartTransform.GetLocation());
	    Platform->SetActorRotation(StartTransform.GetRotation());
    }
}

void APingPongPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAxis("Move", this,&APingPongPlayerController::MoveRight);
}

void APingPongPlayerController::UpdateWidgetBallPower_Implementation(int32 Power)
{
	Widget->UpdateBallPower(Power);
}

void APingPongPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APingPongPlayerController, PlayerID, COND_SimulatedOnly);
}

void APingPongPlayerController::UpdateWidgetPlayerScore_Implementation(int32 Score)
{
	Widget->UpdatePlayerScore(Score);
}

void APingPongPlayerController::UpdateWidgetEnemyScore_Implementation(int32 Score)
{
	Widget->UpdateEnemyScore(Score);
}

void APingPongPlayerController::Client_SetHUDWindow_Implementation(PlayerWindowId windowId)
{
	if (Widget)
	{
		Widget->SwitchWidget(static_cast<int32>(windowId));
	}
}

void APingPongPlayerController::Client_InitializeHUD_Implementation()
{
	if (!Widget)
	{
		Widget = CreateWidget<UPlayerScoresWidget>(this, WidgetClass);
		if (Widget)
		{
			Widget->UpdatePlayerScore(0);
			Widget->UpdateEnemyScore(0);
			Widget->AddToViewport();
		}
	}
}

bool APingPongPlayerController::Client_InitializeHUD_Validate()
{
	return (WidgetClass != nullptr);
}

void APingPongPlayerController::Server_Initialize_Implementation(int32 NewPlayerID, APingPongGate* NewGate)
{
	if (Platform)
	{
		Platform->Destroy();
	}
	
	SpawnPlatform(PlatformClass);

	PlayerID = NewPlayerID;
	Gate = NewGate;
	Gate->SetPlayerID(PlayerID);
}

bool APingPongPlayerController::Server_Initialize_Validate(int32 NewPlayerID, APingPongGate* NewGate)
{
	return (NewPlayerID != 0 && NewGate != nullptr);
}
//DLC
void APingPongPlayerController::SetPlatformSkin(FSoftObjectPath SkinMaterialAsset)
{
	Server_SetPlatformSkin(SkinMaterialAsset);
}

void APingPongPlayerController::SetBallSkin(FSoftObjectPath SkinMaterialAsset)
{
	Server_SetBallSkin(SkinMaterialAsset);
}

void APingPongPlayerController::SetGateSkin(FSoftObjectPath SkinMaterialAsset)
{
	Server_SetGateSkin(SkinMaterialAsset);
}

void APingPongPlayerController::Server_SetPlatformSkin_Implementation(FSoftObjectPath SkinMaterialAsset)
{
	if (SkinMaterialAsset.IsValid())
	{
		auto SkinMaterial { Cast<UMaterial>(SkinMaterialAsset.TryLoad()) };
		if (SkinMaterial && Platform)
		{
			Platform->SetSkin(SkinMaterial);
		}
	}
}

void APingPongPlayerController::Server_SetBallSkin_Implementation(FSoftObjectPath SkinMaterialAsset)
{
	if (SkinMaterialAsset.IsValid())
	{
		auto SkinMaterial { Cast<UMaterial>(SkinMaterialAsset.TryLoad()) };
		if (SkinMaterial && Ball)
		{
			Ball->SetSkin(SkinMaterial);
		}
	}
}

void APingPongPlayerController::Server_SetGateSkin_Implementation(FSoftObjectPath SkinMaterialAsset)
{
	if (SkinMaterialAsset.IsValid())
	{
		auto SkinMaterial { Cast<UMaterial>(SkinMaterialAsset.TryLoad()) };
		if (SkinMaterial && Gate)
		{
			Gate->SetSkin(SkinMaterial);
		}
	}
}