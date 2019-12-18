// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cubemon.generated.h"

UCLASS()
class FINALPRACTICE_API ACubemon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubemon();
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticMesh;
	class UStaticMeshComponent* StaticMeshTwo;
	UPROPERTY(VisibleAnywhere) class UWidgetComponent* WidgetComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float HP = 1.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void ChangeHP(float Value);

	FORCEINLINE bool operator<(const ACubemon& Other) const
	{
		return HP < Other.HP;
	}
};

struct FSortByHP {

	FORCEINLINE bool operator()(const ACubemon& A, const ACubemon& B) const {
		return A.HP < B.HP;
	}

};

struct FSortByDistance {

	FVector Location;

	FSortByDistance(const FVector& Source) {
		Location = Source;
	}

	FORCEINLINE bool operator()(const ACubemon& A, const ACubemon& B) const {

		auto distanceToA = FVector::Distance(Location, A.GetTargetLocation());
		auto distanceToB = FVector::Distance(Location, B.GetTargetLocation());
		return distanceToA < distanceToB;
	}
};
