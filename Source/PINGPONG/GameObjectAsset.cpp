#include "GameObjectAsset.h"

FString FGameObjectInfo::GetGameObjectReference()
{
	if(!PrimaryAssetLabel)
	{
		PrimaryAssetLabel = Cast<UPrimaryAssetLabel>(FStringAssetReference(PrimaryAsset).TryLoad());
	}
	
	if (PrimaryAssetLabel && PrimaryAssetLabel->ExplicitAssets.Num() > 0)
	{
		return  PrimaryAssetLabel->ExplicitAssets[0].GetLongPackageName();
	}
	
	auto AssetPath { PrimaryAsset.GetAssetPathString() };
	int32 DotPoint { AssetPath.Find(".") };
	AssetPath = AssetPath.Right(AssetPath.Len() - DotPoint -1);
	
	return AssetPath;
}