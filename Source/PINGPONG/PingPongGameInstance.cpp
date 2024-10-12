// Fill out your copyright notice in the Description page of Project Settings.


#include "PingPongGameInstance.h"
#include "DLCLoader.h"

TArray<FGameObjectInfo> UPingPongGameInstance::GetGameObjectsInfo()
{
	return DLCLoader->GetGameObjectsInfo();
}

// --------------------------------------------------------------------------------------
void UPingPongGameInstance::Init()
{
	DLCLoader = NewObject<UDLCLoader>(this, "DLC_Loader");
}