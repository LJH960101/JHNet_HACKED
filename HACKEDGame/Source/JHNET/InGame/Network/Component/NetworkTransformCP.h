// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "JHNET.h"
#include "Components/ActorComponent.h"
#include "NetworkBaseCP.h"
#include "NetworkTransformCP.generated.h"
#define SYNC_BY_2_INTERPOLATION

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JHNET_API UNetworkTransformCP : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// 전송 속도 / ms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float sendRate = 0.05f;

	// 보간 속도 / ms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float interporationSpeed = 0.05f;

	// 한계 거리 (이 거리를 넘어서면 텔레포트 한다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float teleportLimit = 100.0f;

	// 이동 격차 (이 거리를 넘어서 이동하면 전송한다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float sendLocationGab = 1.0f;

	// 회전 격차 (이 회전을 넘어서 회전하면 전송한다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float sendRotationGab = 1.0f;

	// 컨트롤 회전 격차 (이 회전을 넘어서 회전하면 전송한다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	float sendControlRotationGab = 5.0f;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetSyncEnable(bool isOn);

	UFUNCTION(BlueprintPure, Category = "Network")
	bool GetSyncEnable();

	// 일정 시간동안 동기화를 강제로 끄는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Network")
	void PlayDisableSyncTimer(float time);
	
private:
	APawn* _owningPawn;
	const float _syncDemantion = 2;
	TArray<FVector> _beforePoses;
	bool _onSync = true;
	bool _onSyncLocation = false;
	bool _onSyncRotation = false;
	bool _isPlayerAuthority;
	FVector _savedLocation;
	FVector _savedRotation;
	FVector _recvLocation;
	FVector _recvRotation;
	FVector _lastSendControlRotation;
	float _timer = 0.f;
	float _locationSyncTimer = 0.f;
	float _rotationSyncTimer = 0.f;
	class UNetworkBaseCP* NetBaseCP = nullptr;
	void _UpdateTransform(float DeltaTime);

	// Disable Sync
	FTimerHandle _disableSyncTimer;
	void _OnDisableSyncEnd();
	   
public:	
	// Sets default values for this component's properties
	UNetworkTransformCP();

	RPC_FUNCTION(UNetworkTransformCP, __RPCSyncLocation, FVector)
	void __RPCSyncLocation(FVector location);

	RPC_FUNCTION(UNetworkTransformCP, __RPCSyncRotation, FVector)
	void __RPCSyncRotation(FVector rotation);

	RPC_FUNCTION(UNetworkTransformCP, __RPCSyncControlRotation, FVector)
	void __RPCSyncControlRotation(FVector targetVector);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
};
