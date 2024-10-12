// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryAssetLabel.h"
#include "UObject/NoExportTypes.h"
#include "MapInfoAsset.generated.h"





USTRUCT(BlueprintType, Blueprintable)
struct FMapInfo
{
	GENERATED_BODY()

	FString GetLevelName() const;
	FText GetLevelDescription() const;
	FString GetLevelReference();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSoftObjectPath PrimaryAsset;

	UPROPERTY()
	UPrimaryAssetLabel * PrimaryAssetLable;
};

UCLASS()
class PINGPONG_API UMapInfoBlueprintFunctionLibrary: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*Blueprint Function to return the value from the struct*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MapInfo|Methods")
	static FString GetLevelReference(UPARAM(ref) FMapInfo& MyStructRef) {return MyStructRef.GetLevelReference(); };
};

UCLASS(BlueprintType, Blueprintable)
class PINGPONG_API UMapInfoAsset : public UObject
{
	GENERATED_BODY()

public:
	UMapInfoAsset(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

	UPROPERTY(EditAnywhere)
	FMapInfo _mapInfo;
};
