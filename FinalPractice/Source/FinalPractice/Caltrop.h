// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Caltrop.generated.h"

UCLASS()
class FINALPRACTICE_API ACaltrop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACaltrop();

	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere) float DamageValue = 0.05f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
