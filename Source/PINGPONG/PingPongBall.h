#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Actor.h"
#include "PingPongBall.generated.h"


UCLASS()
class PINGPONG_API APingPongBall : public AActor
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
    USphereComponent* BodyCollision;
	
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> BodyMeshRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UMaterial> BodyMeshMaterialRef;

	TSharedPtr<FStreamableHandle> AssetHandle;
	TSharedPtr<FStreamableHandle> AssetHandleEffect;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball params")
	float MoveSpeed = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball params")
	UParticleSystem* HitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UParticleSystem> HitEffectRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball params")
	USoundBase*	SoundEffect;
	
	UPROPERTY(Replicated)
	bool IsMoving = false;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(float DeltaTime);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartMove();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopMove();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HitEffect();

public:
	
	APingPongBall();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
    void StartMove();
	
    UFUNCTION(BlueprintCallable)
    void StopMove();
	
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > &OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void SetSkin(UMaterial *Skin);
	
protected:

	void LoadBodyMesh();
	void OnBodyMeshLoaded();

	UMaterial* LoadBodyMeshMaterial();

	void LoadHitEffect();
	void OnHitEffectLoaded();
};
