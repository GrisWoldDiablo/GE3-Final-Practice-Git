// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealKit.generated.h"

UCLASS()
class FINALPRACTICE_API AHealKit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealKit();
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere) class USoundBase* PickupSound;
	UPROPERTY(EditAnywhere) float Duration;
	float HealStartTime;
	bool Heal = false;
	float HPToHeal;
	class AFinalPracticeCharacter* Character;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
