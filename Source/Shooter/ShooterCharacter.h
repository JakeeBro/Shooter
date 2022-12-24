// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "InputAction.h"
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

	/*Move Forward Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enhanced Input", meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveForwardAction;
	
	/*Move Right Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enhanced Input", meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveRightAction;

	/*Turn Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enhanced Input", meta=(AllowPrivateAccess = "true"))
	class UInputAction* TurnAction;
	
	/*Look Up / Down Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enhanced Input", meta=(AllowPrivateAccess = "true"))
	class UInputAction* LookUpAction;

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
	
public:

	/*Returns CameraBoom subobject*/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/*Returns FollowCamera subobject*/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
