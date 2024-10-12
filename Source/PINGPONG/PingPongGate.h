#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongGate.generated.h"

UCLASS()
class PINGPONG_API APingPongGate : public AActor
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* BodyCollision;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* BodyMesh;

	UPROPERTY(Replicated)
	int32 PlayerID { 0 }; 

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Goal();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerID(int32 ID);

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayerStart* PlayerStart;
		
	APingPongGate();
	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Goal();

	void SetPlayerID(int32 ID);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void SetSkin(UMaterial *Skin);
};
