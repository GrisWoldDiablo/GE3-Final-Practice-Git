// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProximityMine.generated.h"

UCLASS()
class FINALPRACTICE_API AProximityMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProximityMine();
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere) class USphereComponent* SphereComponent;
	UPROPERTY(EditAnywhere, category="Default") float Distance = 500.0f;
	UPROPERTY(EditAnywhere, category="Default") float Delay = 3.0f;
	UPROPERTY(EditAnywhere, category="Default") float FlashSpeed = 1.7f;
	UPROPERTY(EditAnywhere, category="Default") class UParticleSystem* Particle;
	UPROPERTY(EditAnywhere, category="Default") float DamageValue = 0.5f;
	class AFinalPracticeCharacter* Character;
	class UMaterialInstanceDynamic* MaterialInstance;
	bool Start = false;
	float StartTime;

	// TIMER!!
	FTimerHandle PulseTimerHandle;
	UPROPERTY(EditAnywhere, category="Default") float WaitStart = -1.0f;
	UPROPERTY(EditAnywhere, category="Default") float PulseSpeed = 1.0f;
	
	UFUNCTION() void Pusle();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void StartTimer();
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void OnConstruction(const FTransform& Transform) override;

};
