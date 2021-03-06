// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "NetworkTransformCP.h"
#include "NetworkModule/Serializer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Common/Network/CommonNetworkProcessor.h"
#include "InGame/Network/InGameNetworkProcessor.h"
#include "Core/Network/NetworkSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"

using namespace JHNETSerializer;

#ifndef SYNC_BY_2_INTERPOLATION
FVector GetVelocity(const FVector& s, const FVector& d)
{
	FVector newVector = FVector::ZeroVector;
	newVector.X = d.X - s.X;
	newVector.Y = d.Y - s.Y;
	newVector.Z = d.Z - s.Z;

	return newVector;
}

float CalcFactorial(float val)
{
	float result = 1.f;
	for (int i = 1; i <= val; ++i) {
		result *= i;
	}
	return result;
}

float CalcCombination(float n, float i)
{
	return CalcFactorial(n) / (CalcFactorial(i) * CalcFactorial(n - i));
}

FVector CalcInterporation(const TArray<FVector>& points, float t)
{
	if (points.Num() <= 0) return FVector::ZeroVector;

	int n = points.Num() - 1;
	FVector sum = FVector::ZeroVector;
	for (int i = 0; i <= n; ++i) {
		sum += CalcCombination(n, i) *
			points[i] *
			FMath::Pow((1 - t), n - i) *
			FMath::Pow(t, i);
	}
	return sum;
}
#endif

// Sets default values for this component's properties
UNetworkTransformCP::UNetworkTransformCP()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	_recvLocation = FVector::ZeroVector;
	_recvRotation = FVector::ZeroVector;
}


// Called when the game starts
void UNetworkTransformCP::BeginPlay()
{
	Super::BeginPlay();

	_owningPawn = Cast<APawn>(GetOwner());
	_recvLocation = GetOwner()->GetActorLocation();
	_recvRotation = GetOwner()->GetActorRotation().Euler();
	for (int i = 0; i < _syncDemantion; ++i)
		_beforePoses.Add(GetOwner()->GetActorLocation());
}


// Called every frame
void UNetworkTransformCP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	JHNET_CHECK(NetBaseCP != nullptr);
	// have sync authority
	if (NetBaseCP->HasAuthority()) {
		if (!_onSync) return;
		_timer -= DeltaTime;
		if (_timer <= 0.f) {
			// send new location and rotation
			_timer += sendRate;

			// send location
			FVector location = GetOwner()->GetActorLocation();
			if (!location.Equals(_savedLocation, sendLocationGab)) {
				__RPCSyncLocation(location);
			}

			FVector rotation = GetOwner()->GetActorRotation().Euler();
			if (!rotation.Equals(_savedRotation, sendRotationGab)) {
				__RPCSyncRotation(rotation);
			}

			if (_owningPawn) {
				FVector controlRotationVector = _owningPawn->GetControlRotation().Euler();
				if (!controlRotationVector.Equals(_lastSendControlRotation, sendControlRotationGab)) __RPCSyncControlRotation(controlRotationVector);
			}
		}
	}
	else _UpdateTransform(DeltaTime);
}

void UNetworkTransformCP::__RPCSyncControlRotation(FVector targetVector)
{
	RPC(NetBaseCP, UNetworkTransformCP, __RPCSyncControlRotation, ENetRPCType::MULTICAST, false, targetVector);
	if (NetBaseCP->HasAuthority()) {
		_lastSendControlRotation = targetVector;
		return;
	}
	FRotator newRotator = FRotator::MakeFromEuler(targetVector);
	if (_owningPawn->GetController()) {
		_owningPawn->GetController()->SetControlRotation(newRotator);
	}
}

void UNetworkTransformCP::_UpdateTransform(float DeltaTime)
{
	if (!_onSync) {
		_savedLocation = GetOwner()->GetActorLocation();
		_savedRotation = GetOwner()->GetActorRotation().Euler();
		_recvLocation = GetOwner()->GetActorLocation();
		_recvRotation = GetOwner()->GetActorRotation().Euler();
		return;
	}

	if (_onSyncLocation) {
		_locationSyncTimer += DeltaTime;
		if (_locationSyncTimer > interporationSpeed) {
			_locationSyncTimer = interporationSpeed;
			_onSyncLocation = false;
		}

		// lerp location

#ifdef SYNC_BY_2_INTERPOLATION
		FVector newVector = FVector::ZeroVector;
		newVector.X = FMath::Lerp(_savedLocation.X, _recvLocation.X, _locationSyncTimer / interporationSpeed);
		newVector.Y = FMath::Lerp(_savedLocation.Y, _recvLocation.Y, _locationSyncTimer / interporationSpeed);
		newVector.Z = FMath::Lerp(_savedLocation.Z, _recvLocation.Z, _locationSyncTimer / interporationSpeed);
		GetOwner()->SetActorLocation(newVector, false, nullptr, ETeleportType::ResetPhysics);
#else
		GetOwner()->SetActorLocation(CalcInterporation(_beforePoses, 0.5f + ((_locationSyncTimer / interporationSpeed) * 0.5f)), false, nullptr, ETeleportType::ResetPhysics);
#endif
	}
	if (_onSyncRotation) {
		_rotationSyncTimer += DeltaTime;
		if (_rotationSyncTimer > interporationSpeed) {
			_rotationSyncTimer = interporationSpeed;
			_onSyncRotation = false;
		}

		// lerp rotation
		FQuat savedQuat = FQuat::MakeFromEuler(_savedRotation);
		FQuat newQuat = FQuat::MakeFromEuler(_recvRotation);
		GetOwner()->SetActorRotation(FQuat::Slerp(savedQuat, newQuat, _rotationSyncTimer / interporationSpeed));
	}
}

void UNetworkTransformCP::_OnDisableSyncEnd()
{
	SetSyncEnable(true);
	GetOwner()->GetWorldTimerManager().ClearTimer(_disableSyncTimer);
}

void UNetworkTransformCP::InitializeComponent()
{
	Super::InitializeComponent();
	UNetworkBaseCP* baseCp = Cast<UNetworkBaseCP>(GetOwner()->GetComponentByClass(UNetworkBaseCP::StaticClass()));
	JHNET_CHECK(baseCp != nullptr);
	_isPlayerAuthority = baseCp->IsPlayerAuthority();

	NetBaseCP = baseCp;

	BindRPCFunction(NetBaseCP, UNetworkTransformCP, __RPCSyncLocation);
	BindRPCFunction(NetBaseCP, UNetworkTransformCP, __RPCSyncRotation);
	BindRPCFunction(NetBaseCP, UNetworkTransformCP, __RPCSyncControlRotation);
}

void UNetworkTransformCP::SetSyncEnable(bool isOn)
{
	_onSync = isOn;
}

bool UNetworkTransformCP::GetSyncEnable()
{
	return _onSync;
}

void UNetworkTransformCP::PlayDisableSyncTimer(float time)
{
	GetOwner()->GetWorldTimerManager().SetTimer(_disableSyncTimer, this, &UNetworkTransformCP::_OnDisableSyncEnd, time, false);
	SetSyncEnable(false);
}

void UNetworkTransformCP::__RPCSyncLocation(FVector location)
{
	RPC(NetBaseCP, UNetworkTransformCP, __RPCSyncLocation, ENetRPCType::MULTICAST, false, location);
	if (NetBaseCP->HasAuthority()) {
		_savedLocation = GetOwner()->GetActorLocation();
		return;
	}

	FVector& recvLocation = location;

	// Refresh recv value
	if (FVector::Dist(recvLocation, GetOwner()->GetActorLocation()) >= teleportLimit) {
		_recvLocation = recvLocation;
		GetOwner()->SetActorLocation(_recvLocation, false, nullptr, ETeleportType::ResetPhysics);

		for (int i = 0; i < _beforePoses.Num(); ++i)
			_beforePoses[i] = _recvLocation;
	}
	else {
		_recvLocation = recvLocation;

		for (int i = 0; i < _beforePoses.Num(); ++i) {
			if (i == _beforePoses.Num() - 1) _beforePoses[i] = _recvLocation;
			else if (i == _beforePoses.Num() - 2) _beforePoses[i] = GetOwner()->GetActorLocation();
			else _beforePoses[i] = _beforePoses[i + 1];
		}
	}

	// Refresh saved value
	_savedLocation = GetOwner()->GetActorLocation();

	_onSyncLocation = true;
	_locationSyncTimer = 0.f;
}

void UNetworkTransformCP::__RPCSyncRotation(FVector rotation)
{
	RPC(NetBaseCP, UNetworkTransformCP, __RPCSyncRotation, ENetRPCType::MULTICAST, false, rotation);
	if (NetBaseCP->HasAuthority()) {
		_savedRotation = GetOwner()->GetActorRotation().Euler();
		return;
	}

	_recvRotation = FVector(rotation.X, rotation.Y, rotation.Z);
	_savedRotation = GetOwner()->GetActorRotation().Euler();

	_onSyncRotation = true;
	_rotationSyncTimer = 0.f;
}