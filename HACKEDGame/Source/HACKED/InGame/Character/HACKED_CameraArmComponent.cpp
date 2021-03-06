// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "HACKED_CameraArmComponent.h"

UHACKED_CameraArmComponent::UHACKED_CameraArmComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ZoomSmoothness = 1.0f;
	DesiredCamRotation = FRotator(0.0f, 0.0f, 0.0f);
}

void UHACKED_CameraArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!FMath::IsNearlyEqual(TargetArmLength, DesiredArmLength, SMALL_NUMBER))
	{
		TargetArmLength = FMath::FInterpTo(TargetArmLength, DesiredArmLength, DeltaTime, ZoomSmoothness);
	}

	if (!SocketOffset.Equals(DesiredSocketOffset, SMALL_NUMBER))
	{
		SocketOffset = FMath::VInterpTo(SocketOffset, DesiredSocketOffset, DeltaTime, OffsetSpeed);
	}	



}

void UHACKED_CameraArmComponent::SetSpringArmComponent()
{
	DesiredArmLength = TargetArmLength;
}

void UHACKED_CameraArmComponent::SetDesiredArmLength(float desiredArmLength, float zoomspeed)
{
	DesiredArmLength = desiredArmLength;
	ZoomSmoothness = zoomspeed;
}

void UHACKED_CameraArmComponent::SetDesiredSocketOffset(FVector socketOffset, float offsetSpeed)
{
	DesiredSocketOffset = socketOffset;
	OffsetSpeed = offsetSpeed;
}

void UHACKED_CameraArmComponent::SetDesiredCamRotation(FRotator desiredRotate, float rotatespeed)
{
	DesiredCamRotation = desiredRotate;
	RotateSpeed = rotatespeed;
}


void UHACKED_CameraArmComponent::CamSmooth(bool zoomOut)
{
	// 차후 추가 되는 수치에 따라 사용 예정 
	DesiredArmLength = zoomOut ? TargetArmLength : TargetArmLength;
	if (DesiredArmLength > MaxTargetLength || DesiredArmLength < MinTargetLength)
	{
		DesiredArmLength = FMath::Min<float>(FMath::Max(DesiredArmLength, MinTargetLength), MaxTargetLength);
	}
}


