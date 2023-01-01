// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	
	/*Begin Play*/
	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	class AShooterCharacter* ShooterCharacter;

	/*The Speed of the Character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float Speed;

	/*Whether or not the Character is in the Air*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsInAir;

	/*Whether ot not the Character is Moving*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsAccelerating;

	/*Offset Yaw used for Strafing*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	float MovementOffsetYaw;

	/*Offset Yaw the frame before we stopped moving*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(AllowPrivateAccess="true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bAiming;
};
