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
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (Pulls in towards Character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // The Camera follows at this Distance behind the Character
	CameraBoom->bUsePawnControlRotation = true;	// Rotate the Arm based on the Controller
	CameraBoom->SocketOffset = FVector(0.f, 70.f, 70.f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach Camera to End of Boom Arm
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;	// Dont Rotate when the Controller Rotates. Let the Controller only affect the Camera
	// bUseControllerRotationYaw = true;	// Rotate when the Controller Rotates. Let the Controller affect Movement
	bUseControllerRotationRoll = false;

	// Configure Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;	// Character Moves in the Direction of Input...
	// GetCharacterMovement()->bOrientRotationToMovement = false;	// Character Moves in relation to Camera Always
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
	
	APawn::AddControllerYawInput(InputValue);
}

void AShooterCharacter::LookUp(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	
	APawn::AddControllerPitchInput(InputValue);
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

		FHitResult FireHit;
		const FVector Start{SocketTransform.GetLocation()};
		const FQuat Rotation{SocketTransform.GetRotation()};
		const FVector RotationAxis{Rotation.GetAxisX()};
		const FVector End{Start + RotationAxis * 50'000.f};

		FVector BeamEndPoint{End};
		
		GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECC_Visibility);

		if (FireHit.bBlockingHit)
		{
			// DrawDebugLine(GetWorld(), Start, FireHit.Location, FColor::Red, false, 2.f);
			// DrawDebugPoint(GetWorld(), FireHit.Location, 10.f, FColor::Red, false, 2.f);

			BeamEndPoint = FireHit.Location;

			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *FireHit.GetActor()->GetName());

			if (ImpactParticles)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.Location);
		}

		if (BeamParticles)
		{
			if (UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform))
				Beam->SetVectorParameter("Target", BeamEndPoint);
		}
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		if (HipFireMontage)
		{
			AnimInstance->Montage_Play(HipFireMontage);
			AnimInstance->Montage_JumpToSection(FName("StartFire"));
		}
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

		Input->BindAction(InputActions->FireButtonAction, ETriggerEvent::Triggered, this, &AShooterCharacter::FireWeapon);
		
		UE_LOG(LogTemp, Warning, TEXT("Player Input Setup Complete"));
	}
}

