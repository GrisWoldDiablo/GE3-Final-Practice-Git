// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FinalPracticeCharacter.h"
#include "FinalPracticeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "PlayerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "Cubemon.h"
#include "Physics/GenericPhysicsInterface.h"
#include "Engine/EngineTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFinalPracticeCharacter

AFinalPracticeCharacter::AFinalPracticeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	if (highChance + lowChance + veryLowChance > 1.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Lootbox chance are not properly set up."))
	}
}

void AFinalPracticeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	auto theWidget = CreateWidget<UPlayerWidget>(UGameplayStatics::GetPlayerController(GetWorld(),0), PlayerWidget);
	
	// Add user widget to viewport, Will Crash if 
	if (theWidget != nullptr)
	{
		theWidget->Player = this;
		theWidget->AddToViewport(0);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Widget not assigned!"));
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void AFinalPracticeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFinalPracticeCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFinalPracticeCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFinalPracticeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFinalPracticeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFinalPracticeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFinalPracticeCharacter::LookUpAtRate);

	// Set the Lootbox key
	PlayerInputComponent->BindAction("Loot", IE_Pressed, this, &AFinalPracticeCharacter::Loot);

	// Set the Special Furthest key
	PlayerInputComponent->BindAction("Furthest", IE_Pressed, this, &AFinalPracticeCharacter::SpecialFurthest);

	// Set Sorted key
	PlayerInputComponent->BindAction("Sorted", IE_Pressed, this, &AFinalPracticeCharacter::Sorted);

	// Set Laser key
	PlayerInputComponent->BindAction("Laser", IE_Pressed, this, &AFinalPracticeCharacter::Laser);
}

void AFinalPracticeCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AFinalPracticeProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AFinalPracticeProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AFinalPracticeCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFinalPracticeCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFinalPracticeCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AFinalPracticeCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AFinalPracticeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFinalPracticeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFinalPracticeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFinalPracticeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AFinalPracticeCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFinalPracticeCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFinalPracticeCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFinalPracticeCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AFinalPracticeCharacter::ChangeHP(float Value)
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

void AFinalPracticeCharacter::Loot()
{
	auto LootNumber = FMath::FRand();
	
	if (LootNumber >= 1.0f - highChance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("You receive a common item!"));
	}
	else if (LootNumber >= 1.0f - highChance - lowChance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, TEXT("You receive a rare item!"));
	}
	else if(LootNumber >= 1.0f - highChance - lowChance - veryLowChance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("You receive a legendary item!"));
	}
	auto test = "bob";
	auto test1 = "bob1";
	auto test2 = "bob2";
	auto test3 = "bob3";
	auto test4 = "bob4";
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("1:%f, 3:%f, 4:%f, 2:%f"),test1, test2, 123, test));

}

void AFinalPracticeCharacter::SpecialFurthest()
{
	auto ObjectQuery = TArray<TEnumAsByte<EObjectTypeQuery>>();
	auto ActorFilter = ACubemon::StaticClass();
	auto ActorToIgnore = TArray<AActor*>();
	auto ActorOut = TArray<AActor*>();

	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), Radius, ObjectQuery, ActorFilter, ActorToIgnore, ActorOut)) {
		ACubemon* FurthestCubemon = nullptr;
		for (auto Actor : ActorOut) {
			auto Cubemon = Cast<ACubemon>(Actor);
			if (Cubemon != nullptr)
			{
				if (FurthestCubemon == nullptr)
				{
					FurthestCubemon = Cubemon;
				}
				else
				{
					auto DistanceFromFurthest = FVector::Distance(GetActorLocation(), FurthestCubemon->GetActorLocation());
					auto DistanceFromCubemon = FVector::Distance(GetActorLocation(), Cubemon->GetActorLocation());
					if (DistanceFromCubemon > DistanceFromFurthest)
					{
						FurthestCubemon = Cubemon;
					}
				}
			}
		}
		if (FurthestCubemon != nullptr)
		{
			FurthestCubemon->HP -= 0.1f;
		}
	}
}

void AFinalPracticeCharacter::Sorted()
{
	auto Actors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACubemon::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		auto Cubemons = TArray<ACubemon*>();
		for (auto Actor : Actors) {
			auto Cubemon = Cast<ACubemon>(Actor);
			if (Cubemon != nullptr)
			{
				Cubemons.Add(Cubemon);
			}
		}
		Cubemons.Sort();
		//Algo::Sort(Cubemons);
		for (auto Cubemon : Cubemons) {
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Cubemon->GetActorLabel() + TEXT(" has ") + FString::SanitizeFloat(Cubemon->HP) + TEXT(" HP"));
		}
	}
}

void AFinalPracticeCharacter::Laser()
{
	auto Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
	if (Camera != nullptr)
	{
		auto ObjQuery = TArray<TEnumAsByte<EObjectTypeQuery>>();
		ObjQuery.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		auto ActorsToIgnore = TArray<AActor*>();
		auto OutHits = TArray<FHitResult>();
		UKismetSystemLibrary::LineTraceMultiForObjects(
			GetWorld(),
			FP_MuzzleLocation->GetComponentLocation(),
			Camera->GetCameraLocation() + (Camera->GetCameraRotation().Vector() * 10000),
			ObjQuery, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits,
			true, FLinearColor::Blue, FLinearColor::Yellow);
		auto Cubemons = TArray<ACubemon*>();
		for (auto OutHit : OutHits) {
			auto Cubemon = Cast<ACubemon>(OutHit.GetActor());
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, OutHit.GetActor()->GetActorLabel());
			if (Cubemon != nullptr)
			{
				
				Cubemons.Add(Cubemon);
			}
		}
		auto count = Cubemons.Num();
		float damage = 0.8f;
		float divider = 1.0f;
		Cubemons.Sort(FSortByDistance(GetActorLocation()));
		for (auto Cubemon : Cubemons) {
			Cubemon->ChangeHP(-Cubemon->HP * damage / divider);
			divider *= 2.0f;
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, Cubemon->GetActorLabel() + TEXT(" : ") + FString::SanitizeFloat(Cubemon->HP));
		}
	}

}
