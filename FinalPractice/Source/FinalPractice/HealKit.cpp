// Fill out your copyright notice in the Description page of Project Settings.


#include "HealKit.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundBase.h"
#include "FinalPracticeCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHealKit::AHealKit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	StaticMesh->SetCollisionProfileName(FName("OverlapAll"));
}

// Called when the game starts or when spawned
void AHealKit::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHealKit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Heal)
	{
		if (HealStartTime + Duration >= GetWorld()->TimeSeconds)
		{
			Character->ChangeHP(HPToHeal * DeltaTime);
			/*if (Character->HP >= 1.0f)
			{
				this->Destroy();
			}*/
		}
		else
		{
			this->Destroy();
		}
	}

	AddActorWorldRotation(FRotator(
		0.0f,				// Pitch
		90.0f * DeltaTime,  // Yaw
		0.0f				// Roll
		));
}

void AHealKit::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Character = Cast<AFinalPracticeCharacter>(OtherActor);

	if (Character != nullptr)
	{
		HPToHeal = (1.0f - Character->HP) / Duration;
		Heal = true;
		HealStartTime =	GetWorld()->TimeSeconds;
		this->SetActorEnableCollision(false);
		this->SetActorHiddenInGame(true);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, Character->GetActorLocation());
	}
}

