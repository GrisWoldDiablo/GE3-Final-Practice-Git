// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

UCLASS()
class FINALPRACTICE_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere) class UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere) float ForwardForce = 35000.0f;
	UPROPERTY(EditAnywhere) float UpForce = 100000.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
