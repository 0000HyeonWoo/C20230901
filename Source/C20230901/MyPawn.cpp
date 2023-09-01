// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MyActorComponent.h"
#include "MyActor.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "Components/InputComponent.h"


// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);
	Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right"));
	Right->SetupAttachment(Body);
	Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left"));
	Left->SetupAttachment(Body);

	Right->SetRelativeLocation(FVector(37.284207f, 20.894921f, 0.589304f));
	Left->SetRelativeLocation(FVector(37.284207f, -20.894921f, 0.589304f));


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);
	Arrow->AddLocalOffset(FVector(100.f, 0, 0));

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));

	MyComponent = CreateDefaultSubobject<UMyActorComponent>(TEXT("MyComponent"));

	BoostSpeed = 0.5f;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	MyComponent->AddSceneComponent(Right);
	MyComponent->AddSceneComponent(Left);

}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddMovementInput(GetActorForwardVector(), BoostSpeed);
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(IA_Boost, ETriggerEvent::Triggered, this, &AMyPawn::Boost);
		EnhancedInputComponent->BindAction(IA_Boost, ETriggerEvent::Completed, this, &AMyPawn::UnBoost);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Triggered, this, &AMyPawn::Fire);
		EnhancedInputComponent->BindAction(IA_PitchAndRoll, ETriggerEvent::Triggered, this, &AMyPawn::PitchAndRoll);
	}
}

void AMyPawn::Boost(const FInputActionValue& Value)
{
	BoostSpeed = 1.f;
}

void AMyPawn::UnBoost(const FInputActionValue& Value)
{
	BoostSpeed = 0.5f;
}

void AMyPawn::Fire(const FInputActionValue& Value)
{
	if (RocketTemplate == NULL)
	{
		return;
	}

	GetWorld()->SpawnActor<AActor>(RocketTemplate,
		Arrow->GetComponentLocation(),
		Arrow->GetComponentRotation());
}

void AMyPawn::PitchAndRoll(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	if (InputValue.IsZero() == true)
	{
		return;
	}

	float Pitch;
	float Roll;

	Pitch = InputValue.X * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 60.f;
	Roll = InputValue.Y * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 60.f;

	FRotator PitchRotator;
	PitchRotator = { Pitch, 0.f, 0.f };

	FRotator RollRotator;
	RollRotator = { 0.f,0.f,Roll };

	AddActorLocalRotation(PitchRotator);
	AddActorLocalRotation(RollRotator);

}

