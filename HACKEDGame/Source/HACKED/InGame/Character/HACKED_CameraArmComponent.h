// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/SpringArmComponent.h"
#include "HACKED_CameraArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UHACKED_CameraArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

	UHACKED_CameraArmComponent();

public:

	class UCameraComponent* FollowCamera;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Camera|Zoom")
	void CamSmooth(bool zoomOut);

	UFUNCTION(BlueprintCallable, Category = "Camera|Zoom")
	void SetSpringArmComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
		float MinTargetLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
		float MaxTargetLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
		float ZoomSmoothness;


	void SetDesiredArmLength(float targetArmLength, float speed = 1.0f);
	void SetDesiredSocketOffset(FVector socketOffset, float offsetSpeed = 5.0f);
	void SetDesiredCamRotation(FRotator desiredRotate, float rotatespeed = 1.0f);

private:

	UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
	float DesiredArmLength;

	UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
	float OffsetSpeed;

	UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
	float RotateSpeed;

	UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
	FVector DesiredSocketOffset;

	UPROPERTY(EditAnywhere, Category = "Camera|Zoom")
	FRotator DesiredCamRotation;
};
