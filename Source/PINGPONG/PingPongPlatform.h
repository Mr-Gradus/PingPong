#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/StreamableManager.h"
#include "PingPongPlatform.generated.h"

UCLASS()
class PINGPONG_API APingPongPlatform : public AActor
{
	GENERATED_BODY()

protected:
		
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BodyCollision;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> BodyMeshRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UMaterial> BodyMeshMaterialRef;

	TSharedPtr<FStreamableHandle> AssetHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform params")
	float MoveSpeed = 100;
	
public:

	APingPongPlatform();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable, WithValidation)
    void Server_MoveRight(float AxisValue);

	UFUNCTION(NetMulticast, Reliable)
	void SetSkin(UMaterial *Skin);
	
private:
	void LoadBodyMesh();
	void OnBodyMeshLoaded();
};
