// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLCLoader.h"
#include "Engine/GameInstance.h"
#include "PingPongGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PINGPONG_API UPingPongGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
    UPROPERTY()
    UDLCLoader* DLCLoader;

public:
	UFUNCTION(BlueprintCallable)
	TArray<FGameObjectInfo> GetGameObjectsInfo();

protected:
	void Init() override;
};
