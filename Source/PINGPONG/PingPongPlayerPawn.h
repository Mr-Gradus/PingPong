#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PingPongPlayerPawn.generated.h"

UCLASS()
class PINGPONG_API APingPongPlayerPawn : public APawn
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category ="Components")
	class USpringArmComponent * SpringArm;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category =	"Components")
	class UCameraComponent * Camera;

public:

	APingPongPlayerPawn();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
