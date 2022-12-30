// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMapData.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

	/*Enhanced Input Context for Mapping Controls*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enhanced Input", meta=(AllowPrivateAccess="true"))
	UInputMappingContext* InputContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enhanced Input", meta=(AllowPrivateAccess="true"))
	UInputMapData* InputActions;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	void MoveForward(const FInputActionValue& Value);
	
	void MoveRight(const FInputActionValue& Value);

	/**
	 * @brief Called via Input to Turn at a given Rate
	 * @param Value Normalized
	 */
	void TurnAtRate(const FInputActionValue& Value);

	/**
	 * @brief Called via Input to Look Up / Down at a given Rate
	 * @param Value Normalized
	 */
	void LookUpAtRate(const FInputActionValue& Value);

	void Turn(const FInputActionValue& Value);

	void LookUp(const FInputActionValue& Value);

	/*Called when the Fire Button is Pressed*/
	void FireWeapon();

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	/*Camera Boom positioning the Camera behind the Character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	/*Camera that follows the Character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;

	/*Base Turn Rate, other Scaling may affect final Turn Rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float BaseTurnRate;

	/*Base Look Up / Down Rate (in Deg/s), other Scaling may affect final Turn Rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	float BaseLookUpRate;

	/*Randomized Gunshot Sound Cue*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class USoundCue* FireSound;

	/*Flash Spawned at "BarrelSocket"*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class UParticleSystem* MuzzleFlash;

	/*Montage for Firing the Weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	class UAnimMontage* HipFireMontage;

	/*Particles Spawned upon Bullet Impact*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UParticleSystem* ImpactParticles;

	/*Smoke Trail for Bullets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	UParticleSystem* BeamParticles;
	
public:

	/*Returns CameraBoom subobject*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/*Returns FollowCamera subobject*/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
