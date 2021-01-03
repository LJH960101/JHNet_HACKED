// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "HackingPanel.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EHackingState : uint8
{
	HACKING_READY = 1,
	HACKING_RUNNING,
	HACKING_END
};

UCLASS()
class HACKED_API AHackingPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	AHackingPanel();
	UPROPERTY(EditAnywhere)
	bool bDoOnceCheck;
	UPROPERTY(EditAnywhere)
	bool bIsRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	UNetworkBaseCP* NetBaseCP;

protected:
	virtual void BeginPlay() override;

//public:
//	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	EHackingState hackingState;
	UPROPERTY(VisibleAnywhere)
	bool bIsPlayerOverlapped;
	UPROPERTY(EditAnywhere)
	bool bIsActive;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* interactionBox;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* hackingPanelComp;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* hologramComp;
	UPROPERTY(VisibleAnywhere)
	UParticleSystem* readyEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystem* runningEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystem* endEffect;
	UPROPERTY(EditAnywhere)
	UPointLightComponent* TEST_light;

	static FColor ReadyColor;
	static FColor RunningColor;
	static FColor EndColor;

	void HackingState();
	void EndHacking();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	RPC_FUNCTION(AHackingPanel, StartHacking)
	void StartHacking();
	
	FORCEINLINE EHackingState GetHackingState() { return hackingState; }
	FORCEINLINE void SetHackingState(EHackingState state) { hackingState = state; }
	FORCEINLINE void SetActiveState(bool state) { bIsActive = state; }
	FORCEINLINE bool GetActiveState() { return bIsActive; }
	FORCEINLINE bool GetRunningState() { return bIsRunning; }
	FORCEINLINE void EndHackingState() { EndHacking(); }
};
