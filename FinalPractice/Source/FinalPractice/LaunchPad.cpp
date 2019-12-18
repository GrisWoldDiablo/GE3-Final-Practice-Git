// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchPad.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "FinalPracticeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ALaunchPad::ALaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 60.0f));
}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaunchPad::NotifyActorBeginOverlap(AActor* OtherActor)
{
	auto Character = Cast<AFinalPracticeCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->GetCharacterMovement()->AddImpulse((Character->GetActorForwardVector() * ForwardForce) + (Character->GetActorUpVector() * UpForce));
	}
}

