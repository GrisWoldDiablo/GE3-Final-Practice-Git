// Fill out your copyright notice in the Description page of Project Settings.


#include "Cubemon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "CubemonHP.h"
#include "FinalPracticeProjectile.h"
#include "kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Engine.h"

// Sets default values
ACubemon::ACubemon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	StaticMeshTwo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Two"));
	StaticMeshTwo->SetupAttachment(RootComponent);
	StaticMeshTwo->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	StaticMeshTwo->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	//WidgetComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}



// Called when the game starts or when spawned
void ACubemon::BeginPlay()
{
	Super::BeginPlay();
	auto CubemonHP = Cast<UCubemonHP>(WidgetComponent->GetUserWidgetObject());
	if (CubemonHP != nullptr)
	{
		CubemonHP->Cubemon = this;
	}

	// Timeline
	FOnTimelineFloat onTimelineCallBack;
	FOnTimelineEventStatic onTimelineFinishedCallback;
	if (FloatCurve != NULL)
	{
		MyTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		MyTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		this->BlueprintCreatedComponents.Add(MyTimeline);
		MyTimeline->SetNetAddressable();
		
		MyTimeline->SetPropertySetObject(this);
		MyTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

		MyTimeline->SetPlaybackPosition(0.0f, false);

		onTimelineCallBack.BindUFunction(this, FName(TEXT("TimelineCallback")));
		onTimelineFinishedCallback.BindUFunction(this, FName(TEXT("TimelineFinishedCallback")));
		MyTimeline->AddInterpFloat(FloatCurve, onTimelineCallBack);
		MyTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		MyTimeline->SetPlayRate(TimelinePlayRate);

		MyTimeline->RegisterComponent();
	}
}

// Called every frame
void ACubemon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (PlayerCamera != nullptr)
	{
		auto PlayerCameraRotation = PlayerCamera->GetCameraRotation();
		WidgetComponent->SetWorldRotation(PlayerCameraRotation);
		WidgetComponent->AddLocalRotation(FRotator(0.0f, 180.0f, 0.0f));
	}

	if (MyTimeline != NULL)
	{
		MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}

void ACubemon::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	auto Projectile = Cast<AFinalPracticeProjectile>(Other);
	if (Projectile != nullptr)
	{
		HP -= HP * 0.1f;
		PlayTimeline();
	}
}

void ACubemon::OnConstruction(const FTransform& Transform)
{
	StaticMeshTwo->SetStaticMesh(StaticMesh->GetStaticMesh());
}

void ACubemon::ChangeHP(float Value)
{
	HP += Value;
	if (HP < 0.0f)
	{
		HP = 0.0f;
	}
	else if (HP > 1.0f)
	{
		HP = 1.0f;
	}
}

// Timeline
UFUNCTION() void ACubemon::TimelineCallback(float val)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Emerald, TEXT("Timeline tick: ") + FString::SanitizeFloat(val));
}

UFUNCTION() void ACubemon::TimelineFinishedCallback()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Timeline Finished!"));
}

void ACubemon::PlayTimeline()
{
	if (MyTimeline != NULL)
	{
		MyTimeline->PlayFromStart();
	}
}

