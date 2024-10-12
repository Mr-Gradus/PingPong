#include "PingPongBall.h"
#include "DrawDebugHelpers.h"
#include "PINGPONGGameModeBase.h"
#include "PingPongGate.h"
#include "PingPongPlayerController.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

APingPongBall::APingPongBall()
{ 	
	PrimaryActorTick.bCanEverTick = true;
	
	BodyCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Ball Body Collider"));
	SetRootComponent(BodyCollision);
	
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Body	Mesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetIsReplicated(true);

	SetReplicates(true);
	SetReplicateMovement(true);
}

void APingPongBall::BeginPlay()
{
	Super::BeginPlay();

	//BodyMesh->SetStaticMesh(LoadBodyMesh());
	LoadBodyMesh();
	BodyMesh->SetMaterial(0, LoadBodyMeshMaterial());
	LoadHitEffect();
	//HitEffect = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"), nullptr, LOAD_None, nullptr);
}

void APingPongBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetNetMode() != ENetMode::NM_Client)
	{
		Server_Move(DeltaTime);
	}
}

void APingPongBall::Server_Move_Implementation(float DeltaTime)
{	
	FVector Forward { GetActorForwardVector() };
	FVector CurrentLocation { GetActorLocation() };
	FVector NewLocation  { CurrentLocation + Forward * MoveSpeed * DeltaTime };
	FHitResult HitResult;
	auto GameMode = Cast<APINGPONGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	

	if (!SetActorLocation(NewLocation, true, &HitResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ball %s Collided with %s"), *GetName(), *HitResult.GetActor()->GetName());

		if (auto PlayerGate = Cast<APingPongGate>(HitResult.GetActor()))
		{
			PlayerGate->Goal();
			if (GameMode != nullptr)
			{
				GameMode->BallPower = 1;
			}
		}
		else
		{
			if (GameMode != nullptr)
			{
				GameMode->BallPower++;
			}
		}

		for (auto Iterator {GetWorld()->GetPlayerControllerIterator()}; Iterator; ++Iterator)
		{
			auto PlayerController { Cast<APingPongPlayerController>(*Iterator) };
			if (PlayerController != nullptr)
			{
				PlayerController->UpdateWidgetBallPower(GameMode->BallPower);	
			}
		}
    	
		FVector MoveVector { CurrentLocation - NewLocation };
		MoveVector.Z = 0;
		MoveVector.Normalize();
    	
		FVector ResetPosition { CurrentLocation + MoveVector * DeltaTime * 2 * MoveSpeed };
		DrawDebugDirectionalArrow(
			GetWorld(),
			ResetPosition + MoveVector * 300,
			ResetPosition,
			30,
			FColor::Green,
			true,
			3.f,
			0,
			3
		);
    	
		FVector ImpactCorrection { HitResult.ImpactPoint + HitResult.ImpactNormal * 300 };
		DrawDebugDirectionalArrow(
			GetWorld(),
			HitResult.ImpactPoint,
			ImpactCorrection,
			30,
			FColor::Blue,
			true,
			3,
			0,
			3
		);
    	
		float AimAtAngle = FMath::RadiansToDegrees(
			acosf(FVector::DotProduct(MoveVector, HitResult.ImpactNormal)));
		MoveVector = MoveVector.RotateAngleAxis(AimAtAngle * 2, FVector(0,0,1));
    	
		FVector NewTargetMove { NewLocation + MoveVector * 300 };
		DrawDebugDirectionalArrow(
			GetWorld(),
			NewLocation,
			NewTargetMove,
			30,
			FColor::Red,
			true,
			3.f,
			0,
			3
		);
    	
		SetActorLocation(ResetPosition);
		FRotator CurrentRotation { GetActorRotation() };
		FRotator NewRotation  { UKismetMathLibrary::FindLookAtRotation(CurrentLocation, NewTargetMove) };
		NewRotation.Pitch = CurrentRotation.Pitch;
		SetActorRotation(NewRotation);
		Multicast_HitEffect();
	}
}

bool APingPongBall::Server_Move_Validate(float DeltaTime)
{
	return IsMoving;
}

void APingPongBall::Server_StartMove_Implementation()
{
	IsMoving = true;
}

bool APingPongBall::Server_StartMove_Validate()
{
	return true;
}

void APingPongBall::Server_StopMove_Implementation()
{
	IsMoving = false;
}

bool APingPongBall::Server_StopMove_Validate()
{
	return true;
}

void APingPongBall::Multicast_HitEffect_Implementation()
{
	UWorld * World = GetWorld();
	
	if(World && SoundEffect && HitEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect,	GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundEffect,	GetActorLocation());
	}
}

void APingPongBall::StartMove()
{

	Server_StartMove();
}

void APingPongBall::StopMove()
{
	Server_StopMove();
}

void APingPongBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(APingPongBall, IsMoving, COND_SimulatedOnly);
}

void APingPongBall::LoadBodyMesh()
{
	FStreamableDelegate LoadMeshDelegate;
	LoadMeshDelegate.BindUObject(this, &APingPongBall::OnBodyMeshLoaded);

	UAssetManager& AssetManager = UAssetManager::Get();
	FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();

	AssetHandle = StreamableManager.RequestAsyncLoad(BodyMeshRef.ToStringReference(), LoadMeshDelegate);
}

void APingPongBall::OnBodyMeshLoaded()
{
	UStaticMesh* LoadedMesh = Cast<UStaticMesh>(AssetHandle.Get()->GetLoadedAsset());
	if (LoadedMesh)
	{
		BodyMesh->SetStaticMesh(LoadedMesh);
	}
}

UMaterial* APingPongBall::LoadBodyMeshMaterial()
{
	if (BodyMeshMaterialRef.IsPending())
	{
		const FSoftObjectPath& AssetRef = BodyMeshMaterialRef.ToStringReference();

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

		BodyMeshRef = Cast<UMaterial>(StreamableManager.LoadSynchronous(AssetRef));
	}
	return  BodyMeshMaterialRef.Get();

}

void APingPongBall::LoadHitEffect()
{
	FStreamableDelegate LoadHitDelegate;
	LoadHitDelegate.BindUObject(this, &APingPongBall::OnHitEffectLoaded);

	UAssetManager& AssetManager = UAssetManager::Get();
	FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();

	AssetHandleEffect = StreamableManager.RequestAsyncLoad(HitEffectRef.ToStringReference(), LoadHitDelegate);
}

void APingPongBall::OnHitEffectLoaded()
{
	UParticleSystem* LoadedHitEffect = Cast<UParticleSystem>(AssetHandleEffect.Get()->GetLoadedAsset());
	if (LoadedHitEffect)
	{
		HitEffect = LoadedHitEffect;
	}
}

void APingPongBall::SetSkin_Implementation(UMaterial* Skin)
{
	if (BodyMesh)
	{
		BodyMesh->SetMaterial(0, Skin);
	}
}