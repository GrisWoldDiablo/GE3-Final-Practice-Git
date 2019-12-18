// Fill out your copyright notice in the Description page of Project Settings.


#include "Caltrop.h"
#include "FinalPracticeCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"

// Sets default values
ACaltrop::ACaltrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = StaticMesh;
	StaticMesh->SetSimulatePhysics(true);
	//Uncomment to set default mesh.
	//auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
	//UStaticMesh* MyMesh = MeshAsset.Object;
	//StaticMesh->SetStaticMesh(MyMesh);
}

// Called when the game starts or when spawned
void ACaltrop::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACaltrop::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	auto Player = Cast<AFinalPracticeCharacter>(Other);
	if (Player != nullptr)
	{
		Player->ChangeHP(-DamageValue);
		Destroy();
	}
}

// Called every frame
void ACaltrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

