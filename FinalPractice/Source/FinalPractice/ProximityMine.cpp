// Fill out your copyright notice in the Description page of Project Settings.


#include "ProximityMine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "FinalPracticeCharacter.h"
#include "kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine.h"
#include "TimerManager.h"

// Sets default values
AProximityMine::AProximityMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->InitSphereRadius(Distance);
	SphereComponent->SetCollisionProfileName(FName("OverlapAll"));
	
}

// TIMER!!
UFUNCTION() void AProximityMine::Pusle()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Magenta, TEXT("Timer!!"));
}

// Called when the game starts or when spawned
void AProximityMine::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<AFinalPracticeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is NULL on BeginPlay!"));
	}
	MaterialInstance = StaticMesh->CreateDynamicMaterialInstance(0);
	GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &AProximityMine::Pusle, PulseSpeed, true, WaitStart);
}

// Called every frame
void AProximityMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (!Start)
	{
		if (Character != nullptr)
		{
			auto DistanceFromPlayer = FVector::Distance(Character->GetActorLocation(), GetActorLocation());
			if (DistanceFromPlayer <= Distance)
			{
				Start = true;
				StartTime = GetWorld()->TimeSeconds;
				if (MaterialInstance != nullptr)
				{
					MaterialInstance->SetScalarParameterValue(FName("Lerp"), 1.0f);
				}

			}
		}
		else
		{
			Character = Cast<AFinalPracticeCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		}
		
	}
	else*/

	if(Start)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::SanitizeFloat((GetWorld()->TimeSeconds - StartTime)));
		if (MaterialInstance != nullptr)
		{
			auto timeLeft = (GetWorld()->TimeSeconds - StartTime) / Delay * 10.0f;
			auto lerp = (FMath::Sin(pow(FlashSpeed, timeLeft)) + 1.0f) / 2.0f;
			MaterialInstance->SetScalarParameterValue(FName("Lerp"), lerp);
		}

		if (StartTime + Delay <= GetWorld()->TimeSeconds)
		{
			auto ObjectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
			UClass* ActorClassFilter = AFinalPracticeCharacter::StaticClass();
			auto ActorsToIgnore = TArray<AActor*>();
			auto OutActors = TArray<AActor*>();

			if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), Distance, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors))
			{
				for (auto Actor : OutActors) {
					auto Char = Cast<AFinalPracticeCharacter>(Actor);
					if (Char != nullptr)
					{
						Char->ChangeHP(-DamageValue);
					}
				}
			}

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, GetActorTransform());
			Destroy();
		}
	}
}

void AProximityMine::StartTimer()
{
	Start = true;
	StartTime = GetWorld()->TimeSeconds;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::SanitizeFloat((GetWorld()->TimeSeconds)));
	/*if (MaterialInstance != nullptr)
	{
		MaterialInstance->SetScalarParameterValue(FName("Lerp"), 1.0f);
	}*/
}

void AProximityMine::NotifyActorBeginOverlap(AActor* OtherActor)
{
	auto player = Cast<AFinalPracticeCharacter>(OtherActor);
	if (player != nullptr && !Start)
	{
		StartTimer();
	}
}

void AProximityMine::OnConstruction(const FTransform& Transform)
{
	SphereComponent->SetSphereRadius(Distance);
}

