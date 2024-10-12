#include "MapInfoAsset.h"

FString FMapInfo::GetLevelName() const
{
	return Name.ToString();
}
FText FMapInfo::GetLevelDescription() const
{
	return Description;
}

FString FMapInfo::GetLevelReference()
{
	if(!PrimaryAssetLable)
	{
		PrimaryAssetLable = Cast<UPrimaryAssetLabel>(FStringAssetReference(PrimaryAsset).TryLoad());
	}

	if (PrimaryAssetLable && PrimaryAssetLable->ExplicitAssets.Num() > 0)
		PrimaryAssetLable;
	FString assetPath = PrimaryAsset.GetAssetPathString();
	
    int32 dotPoint = assetPath.Find(".");
    assetPath = assetPath.Right(assetPath.Len() - dotPoint -1);
    return assetPath;
}
