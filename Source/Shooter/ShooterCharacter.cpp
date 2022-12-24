// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (Pulls in towards Character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // The Camera follows at this Distance behind the Character
	CameraBoom->bUsePawnControlRotation = true;	// Rotate the Arm based on the Controller

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to End of Boom Arm
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() Called!"));

	constexpr int MyInt{42};
	UE_LOG(LogTemp, Warning, TEXT("int MyInt: %d"), MyInt);

	constexpr float MyFloat{3.14159f};
	UE_LOG(LogTemp, Warning, TEXT("float MyFloat: %f"), MyFloat);

	constexpr double MyDouble{0.000756};
	UE_LOG(LogTemp, Warning, TEXT("double MyDouble: %lf"), MyDouble);

	constexpr char MyChar{'J'};
	UE_LOG(LogTemp, Warning, TEXT("char MyChar: %c"), MyChar);

	constexpr wchar_t WideChar{L'J'};
	UE_LOG(LogTemp, Warning, TEXT("wchar_t WideChar: %lc"), WideChar);

	constexpr bool MyBool{true};
	UE_LOG(LogTemp, Warning, TEXT("bool MyBool: %d"), MyBool);

	UE_LOG(LogTemp, Warning, TEXT("int: %d, float: %f, bool: %d"), MyInt, MyFloat, MyBool);

	const FString MyString{TEXT("My String!")};
	UE_LOG(LogTemp, Warning, TEXT("string MyString: %s"), *MyString);

	UE_LOG(LogTemp, Warning, TEXT("Instance: %s"), *GetName());
	*/
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContext, 0);

			UE_LOG(LogTemp, Warning, TEXT("Mapping Context Set"));
		}
	}
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterCharacter::MoveForward(const FInputActionValue& Value)
{
	if (Controller == nullptr || Value.Get<float>() == 0.f) return;
	
	float InputValue = Value.Get<float>();

	const FRotator Rotation{Controller->GetControlRotation()};
	const FRotator YawRotation{0, Rotation.Yaw, 0};
	const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};
	AddMovementInput(Direction, InputValue);
}

void AShooterCharacter::MoveRight(const FInputActionValue& Value)
{
	if (Controller == nullptr || Value.Get<float>() == 0.f) return;
	
	float InputValue = Value.Get<float>();

	const FRotator Rotation{Controller->GetControlRotation()};
	const FRotator YawRotation{0, Rotation.Yaw, 0};
	const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};
	AddMovementInput(Direction, InputValue);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveForward);
		Input->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveRight);
		
		UE_LOG(LogTemp, Warning, TEXT("Player Input Setup Complete"));
	}
}

