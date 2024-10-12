#include "PINGPONGGameModeBase.h"
#include "PingPongPlayerController.h"
#include "PingPongPlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "PingPongGate.h"
#include "PingPongBall.h"

namespace utils
{
	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}
}

APINGPONGGameModeBase::APINGPONGGameModeBase()
{
	DefaultPawnClass = APingPongPlayerPawn::StaticClass();
	PlayerControllerClass = APingPongPlayerController::StaticClass();
}

void APINGPONGGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void APINGPONGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
 
	UWorld* World = GetWorld();
	
	if(World && (!Player1Start || !Player2Start))
	{
		TArray<APlayerStart*> FoundActors;
		utils::FindAllActors<APlayerStart>(World, FoundActors);
		if(FoundActors.Num() > 0)
		{
			Player1Start = FoundActors[0];
		}
		if(FoundActors.Num() > 1)
		{
			Player2Start = FoundActors[1];
		}
	}
	
	APingPongPlayerController* CurrentPlayer;
	
	APlayerStart* StartPosition;

	int32 PlayerID;
	
	if(Player1 == nullptr)
	{
		Player1 = Cast<APingPongPlayerController>(NewPlayer);
		PlayerID = 1;
		CurrentPlayer = Player1;
		StartPosition = Player1Start;
		
		UE_LOG(LogTemp, Warning, TEXT("PingPongGameMode: Init player1"));
	}
	else if(Player2 == nullptr)
	{
		Player2 = Cast<APingPongPlayerController>(NewPlayer);
		PlayerID = 2;
		CurrentPlayer = Player2;
		StartPosition = Player2Start;
		
		UE_LOG(LogTemp, Warning, TEXT("PingPongGameMode: Init player2"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PingPongGameMode: There are already two players in the game. New connections are not possible"));
		return;
	}
	
	APingPongPlayerPawn* NewPawn = Cast<APingPongPlayerPawn>(NewPlayer->GetPawn());
	
	if(!NewPawn)
    {
		NewPawn = World->SpawnActor<APingPongPlayerPawn>(DefaultPawnClass);
    }

	TArray<APingPongGate*> FoundGateActors;
	utils::FindAllActors<APingPongGate>(World, FoundGateActors);
	auto FindGateByPlayerStart = [](const TArray<APingPongGate*>& Array, const APlayerStart* SearchObject)
	{
		auto Result = Array.FindByPredicate([&](const APingPongGate *Gate)
		{
			return Gate->PlayerStart == SearchObject;
		});
		return Result != nullptr ? * Result : nullptr;
	};
	
    if(NewPawn)
    {
    	auto PlayerGate { FindGateByPlayerStart(FoundGateActors, StartPosition) };
	    NewPawn->SetActorLocation(StartPosition->GetActorLocation());
	    NewPawn->SetActorRotation(StartPosition->GetActorRotation());
	    NewPlayer->SetPawn(NewPawn);
	    CurrentPlayer->SetStartTransform(StartPosition->GetActorTransform());	   	
    	CurrentPlayer->Client_InitializeHUD();
    	CurrentPlayer->Server_Initialize(PlayerID, PlayerGate);
    	
    	if (Player1 != nullptr && Player2 != nullptr)
    	{
    		Player1->Client_SetHUDWindow(PlayerWindowId::Game);
    		Player2->Client_SetHUDWindow(PlayerWindowId::Game);
	   			StartGame();
    	}
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("Start position not setted in PingPongGameMode!"));
    }
}

void APINGPONGGameModeBase::PlayerGoal(int32 PlayerID)
{
	auto CurrentPlayerScore { 0 };
	
	if (PlayerID == 1)
	{
		CurrentPlayerScore = Player2Score + BallPower;
		Player2Score = CurrentPlayerScore;
	}
	else if (PlayerID == 2)
	{
		CurrentPlayerScore = Player1Score + BallPower;
		Player1Score = CurrentPlayerScore;
	}
	
	for (auto Iterator {GetWorld()->GetPlayerControllerIterator()}; Iterator; ++Iterator)
	{
		auto PlayerController { Cast<APingPongPlayerController>(*Iterator) };
		if (PlayerController != nullptr)
		{
			if (PlayerController->GetPlayerID() == PlayerID)
			{
				PlayerController->UpdateWidgetEnemyScore(CurrentPlayerScore);
			}
			else
			{
				PlayerController->UpdateWidgetPlayerScore(CurrentPlayerScore);				
			}			
		}
	}
}

bool APINGPONGGameModeBase::StartGame()
{
	TArray<APingPongBall*> FoundActors;
	utils::FindAllActors<APingPongBall>(GetWorld(), FoundActors);
	
	if (FoundActors.Num() > 0)
	{
		APingPongBall* Ball { FoundActors.Last() };
		Ball->StartMove();
		return true;
	}
	return false;
}
