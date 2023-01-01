// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	// Base Rates for Turning / Looking Up
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// Turn Rates for Aiming / Not Aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimTurnRate(20.f),
	AimLookUpRate(20.f),
	// Mouse Look Sensitivity Scale Factors
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimTurnRate(.2f),
	MouseAimLookUpRate(.2f),
	// True when Aiming
	bAiming(false),
	// Camera FOV values
	CameraDefaultFOV(0.f), // Set in BeginPlay()
	CameraZoomedFOV(45.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (Pulls in towards Character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.f; // The Camera follows at this Distance behind the Character
	CameraBoom->bUsePawnControlRotation = true;	// Rotate the Arm based on the Controller
	CameraBoom->SocketOffset = FVector(0.f, 70.f, 70.f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to End of Boom Arm
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = false;	// Dont Rotate when the Controller Rotates. Let the Controller only affect the Camera
	bUseControllerRotationYaw = true;	// Rotate when the Controller Rotates. Let the Controller affect Movement
	bUseControllerRotationRoll = false;

	// Configure Character Movement
	// GetCharacterMovement()->bOrientRotationToMovement = true;	// Character Moves in the Direction of Input...
	GetCharacterMovement()->bOrientRotationToMovement = false;	// Character Moves in relation to Camera Always
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ...At this Rotation Rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = .2f;
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
	*/
	
	UE_LOG(LogTemp, Warning, TEXT("Instance: %s"), *GetName());
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContext, 0);

			UE_LOG(LogTemp, Warning, TEXT("Mapping Context Set"));
			
			TArray<FEnhancedActionKeyMapping> Mappings = InputContext->GetMappings();

			for (FEnhancedActionKeyMapping Mapping : Mappings)
			{
				UE_LOG(LogTemp, Warning, TEXT("Input Mapping Name: %s"), *Mapping.Action.GetName());
			}
		}
	}

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InterpolateCameraFOV(DeltaTime);

	BaseTurnRate = bAiming ? AimTurnRate : HipTurnRate;
	BaseLookUpRate = bAiming ? AimLookUpRate : HipLookUpRate;

	CalculateCrosshairSpread(DeltaTime);
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

void AShooterCharacter::TurnAtRate(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	// Calculate Delta for this Frame from the Rate Information
	AddControllerYawInput(InputValue * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	AddControllerPitchInput(InputValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	float TurnScaleFactor = bAiming ? MouseAimTurnRate : MouseHipTurnRate;
	
	APawn::AddControllerYawInput(InputValue * TurnScaleFactor);
}

void AShooterCharacter::LookUp(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	float LookUpScaleFactor = bAiming ? MouseAimLookUpRate : MouseHipLookUpRate;
	
	APawn::AddControllerPitchInput(InputValue * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (FireSound)
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);

	if (const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket"))
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);

		if (FVector BeamEnd; GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd))
		{
			if (ImpactParticles)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);

			if (BeamParticles)
				if (UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform))
					Beam->SetVectorParameter("Target", BeamEnd);
		}
	} 

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		if (HipFireMontage)
		{
			AnimInstance->Montage_Play(HipFireMontage);
			AnimInstance->Montage_JumpToSection(FName("StartFire"));
		}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	// Get Current Size of the Viewport
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
		GEngine->GameViewport->GetViewportSize(ViewportSize);

	// Get Screen Space Location of the Crosshair
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get World Position and Direction of the Crosshair
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(
			this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) // Was Deprojection Successful?
	{
		FHitResult ScreenTraceHit;
		const FVector Start{CrosshairWorldPosition};
		const FVector End{CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f};

		// Set Beam End Point to Line Trace End Point
		OutBeamLocation = End;

		// Trace Outward from the Crosshair World Location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // Was there a Trace Hit?
			OutBeamLocation = ScreenTraceHit.Location; // Beam End Point is now Trace Hit Location

		// Perform a Second Trace, this time from the Gun Barrel
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{MuzzleSocketLocation};
		const FVector WeaponTraceEnd{OutBeamLocation};

		GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);

		// Object between Barrel and Beam End Point
		if (WeaponTraceHit.bBlockingHit)
			OutBeamLocation = WeaponTraceHit.Location;

		return true;
	}

	return false;
}

void AShooterCharacter::InterpolateCameraFOV(float DeltaTime)
{
	// Set Current Field of View
	if (bAiming)
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	else
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{0.f, GetCharacterMovement()->MaxWalkSpeed};
	FVector2D VelocityMultiplierRange{0.f, 1.f};
	FVector Velocity{GetVelocity()};
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
	
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(InputActions->MoveForwardAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveForward);
		Input->BindAction(InputActions->MoveRightAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveRight);

		Input->BindAction(InputActions->TurnRateAction, ETriggerEvent::Triggered, this, &AShooterCharacter::TurnAtRate);
		Input->BindAction(InputActions->LookUpRateAction, ETriggerEvent::Triggered, this, &AShooterCharacter::LookUpAtRate);

		Input->BindAction(InputActions->TurnAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Turn);
		Input->BindAction(InputActions->LookUpAction, ETriggerEvent::Triggered, this, &AShooterCharacter::LookUp);

		Input->BindAction(InputActions->JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		Input->BindAction(InputActions->JumpAction, ETriggerEvent::Canceled, this, &ACharacter::StopJumping);

		Input->BindAction(InputActions->FireButtonAction, ETriggerEvent::Started, this, &AShooterCharacter::FireWeapon);

		Input->BindAction(InputActions->AimStartAction, ETriggerEvent::Triggered, this, &AShooterCharacter::AimStart);
		Input->BindAction(InputActions->AimStopAction, ETriggerEvent::Triggered, this, &AShooterCharacter::AimStop);
		
		UE_LOG(LogTemp, Warning, TEXT("Player Input Setup Complete"));
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

