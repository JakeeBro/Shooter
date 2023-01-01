// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

	if (ShooterCharacter)
	{
		// Get the Lateral Speed of the Character from Velocity
		FVector Velocity{ShooterCharacter->GetVelocity()};
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Is the Character in the Air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is the Character Accelerating?
		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 0.f, FColor::White, FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw));
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw));
			GEngine->AddOnScreenDebugMessage(-2, 0.f, FColor::White, FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw));
		}
		*/

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (ShooterCharacter->GetVelocity().Size() > 0.f)
			LastMovementOffsetYaw = MovementOffsetYaw;

		bAiming = ShooterCharacter->GetAiming();
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
