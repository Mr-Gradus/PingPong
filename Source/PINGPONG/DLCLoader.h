#pragma once

#include "CoreMinimal.h"
#include "GameObjectAsset.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/PakFile/Public/IPlatformFilePak.h"
#include "DLCLoader.generated.h"

class PINGPONG_API FDLCLoaderFileVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
	{
		if (!bIsDirectory)
		{
			Files.Add(FilenameOrDirectory);
		}
		return true;
	}
	
	TArray<FString> Files;
};

UCLASS()
class PINGPONG_API UDLCLoader : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameObjectInfo> GameObjectsInfo;

	FPakPlatformFile* DLCFile { nullptr };
	
#if UE_BUILD_SHIPPING == 0
	IPlatformFile *OriginalPlatformFile { nullptr };
#endif
	
public:
	bool ReadDLCGameObjectsInfo();
	
	TArray<FGameObjectInfo> GetGameObjectsInfo();
	
	void Clear();
	
private:
	TArray<FString> GetClassesToLoad();
	
	TArray<FString> LoadDLC();
	
	// DLC methods
	bool MountDLC(const FString& PakFilename);
	
	bool UnmountDLC(const FString& PakFilename);
	
	int32 GetDLCOrder(const FString & PakFilePath);
	
	void RegisterMountPoint(const FString& RootPath, const FString& ContentPath);
	
	TArray<FString> GetFilesInDLC(const FString& Directory);
	
	bool ReadPakFile(FString PakFileName);
	
	UClass* LoadClassFromDLC(const FString& Filename);
	
	FPakPlatformFile* GetDLCFile();
};
