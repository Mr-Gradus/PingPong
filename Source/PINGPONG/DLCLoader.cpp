#include "DLCLoader.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

bool UDLCLoader::ReadDLCGameObjectsInfo()
{
#if UE_BUILD_SHIPPING
//
#else
	UAssetManager* AssetManager { &UAssetManager::Get() };
	TArray<FString> ClassesToLoad { GetClassesToLoad() };
	TArray<FAssetData> RegisteryData;
	if (AssetManager->GetAssetRegistry().GetAllAssets(RegisteryData))
	{
		for (FAssetData& Data : RegisteryData)
		{
			for (FString ClassToLoad : ClassesToLoad)
			{
				if (Data.AssetName == FName(*ClassToLoad))
				{
					UObject* ObjectToLoad { nullptr };
					auto GeneratedClassPath { Data.TagsAndValues.FindTag(FName("GeneratedClass")) };
					if (GeneratedClassPath.IsSet())
					{
						const FString ClassObjectPath {
							FPackageName::ExportTextPathToObjectPath(*GeneratedClassPath.GetValue()) };
						const FString ClassName { FPackageName::ObjectPathToObjectName(ClassObjectPath) };
						ObjectToLoad = FStringAssetReference(ClassObjectPath).TryLoad();
					}
					if (ObjectToLoad)
					{
						UBlueprintGeneratedClass* GeneratedClass { Cast<UBlueprintGeneratedClass>(ObjectToLoad) };
						UGameObjectAsset * GameObjectAsset {
							Cast<UGameObjectAsset>(GeneratedClass->GetDefaultObject()) };
						if (GameObjectAsset)
						{
							GameObjectsInfo.Add(GameObjectAsset->GameObjectInfo);
						}
					}
				}
			}
		}
	}
#endif
	LoadDLC();
	return GameObjectsInfo.Num() > 0;
}

TArray<FGameObjectInfo> UDLCLoader::GetGameObjectsInfo()
{
	if (GameObjectsInfo.Num() == 0)
	{
		ReadDLCGameObjectsInfo();
	}
	return GameObjectsInfo;
}

void UDLCLoader::Clear()
{
	GameObjectsInfo.Empty();
}

TArray<FString> UDLCLoader::GetClassesToLoad()
{
	TArray<FString> OutClasses;
	TArray<FName> BaseClasses;
	TSet<FName> ExludedClasses;
	TSet<FName> DerivedClasses;
	BaseClasses.Add(UGameObjectAsset::StaticClass()->GetFName());
	UAssetManager* AssetManager { &UAssetManager::Get() };
	AssetManager->GetAssetRegistry().GetDerivedClassNames(
		BaseClasses,
		ExludedClasses,
		DerivedClasses
	);
	
	for (FName Derived : DerivedClasses)
	{
		/*Trim class name to get ClasName instead of ClassName.ClassName_C*/
		FString DerivedString { Derived.ToString() };
		if (DerivedString.EndsWith("_C"))
		{
			DerivedString = DerivedString.Mid(0, DerivedString.Len() - 2);
		}
		OutClasses.Add(*DerivedString);
	}
	return OutClasses;
}

TArray<FString> UDLCLoader::LoadDLC()
{
	TArray<FString> LoadedPlugins;
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Run in Editor mode. DLC loadingaborted"));
	return LoadedPlugins;
#endif
	
	// Get DLC folder
	FString PathToDLCFolder { FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "DLC" };
	UE_LOG(LogTemp, Warning, TEXT("DLC PATH: %s"), *PathToDLCFolder);
	
	IFileManager& FileManager { IFileManager::Get() };
	if (!FPaths::DirectoryExists(PathToDLCFolder))
	{
		UE_LOG(LogTemp, Warning, TEXT("DLC Directory %s not found"), *PathToDLCFolder);
		FileManager.MakeDirectory(*PathToDLCFolder, true);
		return LoadedPlugins;
	}
	
	// Get all *.pak files and try to load plugins
	TArray<FString> PAKFiles;
	FString DLCExtension { "*.pak" };
    FileManager.FindFilesRecursive(
    	PAKFiles,
    	*PathToDLCFolder,
		*DLCExtension,
		true,
		false
	);
    for (size_t i { 0 }; i < PAKFiles.Num(); ++i)
    {
		MountDLC(PAKFiles[i]);
		ReadPakFile(PAKFiles[i]);
	}
	return LoadedPlugins;
}

bool UDLCLoader::MountDLC(const FString& PakFilename)
{
	int32 PakOrder { GetDLCOrder(PakFilename) };
	FPakPlatformFile* DLCPakFile { GetDLCFile() };
	TArray<FString> MountedPoints1;
	DLCPakFile->GetMountedPakFilenames(MountedPoints1);
	bool Result = DLCPakFile->Mount(*PakFilename, PakOrder, nullptr);
	TArray<FString> MountedPoints2;
    DLCPakFile->GetMountedPakFilenames(MountedPoints2);
    return Result;
}

bool UDLCLoader::UnmountDLC(const FString& PakFilename)
{
    FPakPlatformFile* DLCPakFile { GetDLCFile() };
    TArray<FString> MountedPoints1;
    DLCPakFile->GetMountedPakFilenames(MountedPoints1);
    if (MountedPoints1.Contains(PakFilename))
    {
		UE_LOG(LogTemp, Warning, TEXT("Mount point %s exists! Unmount started..."), *PakFilename);
		if (DLCPakFile->Unmount(*PakFilename))
		{
			UE_LOG(LogTemp, Warning, TEXT("Unmounted!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unmounting error!"));
			return false;
		}
    }
    return true;
}

int32 UDLCLoader::GetDLCOrder(const FString& PakFilePath)
{
	if (PakFilePath.StartsWith(FString::Printf(TEXT("%sPaks/%s-"),
		*FPaths::ProjectContentDir(), FApp::GetProjectName())))
    {
		return 4;
    }
    else if (PakFilePath.StartsWith(FPaths::ProjectContentDir()))
    {
		return 3;
    }
    else if (PakFilePath.StartsWith(FPaths::EngineContentDir()))
    {
		return 2;
    }
    else if (PakFilePath.StartsWith(FPaths::ProjectSavedDir()))
    {
		return 1;
    }
    return 0;
}

void UDLCLoader::RegisterMountPoint(const FString& RootPath, const FString& ContentPath)
{
	FPackageName::RegisterMountPoint(RootPath, ContentPath);
}

TArray<FString> UDLCLoader::GetFilesInDLC(const FString& Directory)
{
	FDLCLoaderFileVisitor Visitor;
    GetDLCFile()->IterateDirectory(*Directory, Visitor);
    return Visitor.Files;
}

FPakPlatformFile* UDLCLoader::GetDLCFile()
{
	if (!DLCFile)
	{
		IPlatformFile *CurrentPlatformFile { FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")) };
		if (CurrentPlatformFile)
		{
			DLCFile = static_cast<FPakPlatformFile*>(CurrentPlatformFile);
		}
		else
		{
			DLCFile = new FPakPlatformFile();
			ensure(DLCFile != nullptr);
			IPlatformFile &PlatformFile { FPlatformFileManager::Get().GetPlatformFile() };
#if UE_BUILD_SHIPPING == 0
			OriginalPlatformFile = &PlatformFile;
#endif
			if (DLCFile->Initialize(&PlatformFile, TEXT("")))
			{
				FPlatformFileManager::Get().SetPlatformFile(*DLCFile);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DLCFile initialization error!"));
			}
		}
	}
	ensure(DLCFile != nullptr);
    return DLCFile;
}

bool UDLCLoader::ReadPakFile(FString PakFileName)
{
	UE_LOG(LogTemp, Warning, TEXT("ReadPakFile: Mount pak file : %s"), *PakFileName);
	FPakPlatformFile *PakPlatformFile;
	{
		FString PlatformFileName { FPlatformFileManager::Get().GetPlatformFile().GetName() };
		if (PlatformFileName.Equals(FString(TEXT("PakFile"))))
		{
			PakPlatformFile = static_cast<FPakPlatformFile*>(&FPlatformFileManager::Get().GetPlatformFile());
		}
		else
		{
			PakPlatformFile = new FPakPlatformFile;
			if (!PakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT("")))
			{
				UE_LOG(LogTemp, Error, TEXT("FPakPlatformFile failed to initialize"));
				return false;
			}
			FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
		}
	}
	FString PakFilePathFull { FPaths::ConvertRelativePathToFull(PakFileName) };
	//FPakFile PakFile {PakPlatformFile, *PakFilePathFull, false};
	TRefCountPtr<FPakFile> PakFile { new FPakFile{PakPlatformFile, *PakFilePathFull, false}};
	TArray<FString> FileList;
	FString PackName { FPaths::GetBaseFilename(PakFileName) };
	FString MountPoint { PakFile->GetMountPoint() };
	PakFile->FindPrunedFilesAtPath(FileList, *MountPoint, true, false, true);
	FStreamableManager StreamableManager;
	for (int32 k { 0 }; k < FileList.Num(); ++k)
	{
		FString AssetName { FileList[k] };
		if (AssetName.Contains("BP_") && AssetName.Contains(".uasset"))
		{
			UE_LOG(LogTemp, Log, TEXT("Loading Pak: %s from File %s..."), *PackName, *FileList[k]);
			FString AssetShortName { FPackageName::GetShortName(AssetName) };
			FString FileName, FileExt;
			AssetShortName.Split(TEXT("."), &FileName, &FileExt);
			FString NewAssetName { FString("/") + PackName + "/" + FileName + TEXT(".") + FileName + "_C" };
            UE_LOG(LogTemp, Log, TEXT("Loading Asset %s ..."), *NewAssetName);
            UObject* ObjectToLoad { nullptr };
            FStringAssetReference ref { FStringAssetReference(NewAssetName) };
            UE_LOG(LogTemp, Warning, TEXT("load object..."));
            ObjectToLoad = ref.TryLoad();
            if (ObjectToLoad)
            {
				UBlueprintGeneratedClass* generatedClass { Cast<UBlueprintGeneratedClass>(ObjectToLoad) };
				UGameObjectAsset * GameObjectAsset { Cast<UGameObjectAsset>(generatedClass->GetDefaultObject()) };
				// Take default object by "info" class
				if (GameObjectAsset)
				{
					GameObjectsInfo.Add(GameObjectAsset->GameObjectInfo);
					UE_LOG(LogTemp, Warning, TEXT("Add data %s"), *GameObjectAsset->GetFullName());
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("File %s loading error!"), *FileList[k]);
				}
            }
        }
    }
    return true;
}

UClass* UDLCLoader::LoadClassFromDLC(const FString& Filename)
{
	const FString Name { Filename + TEXT(".") + FPackageName::GetShortName(Filename) + TEXT("_C") };
    return StaticLoadClass(UObject::StaticClass(), nullptr, *Name);
}