// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "PlasmaDoor.generated.h"

UCLASS()
class HACKED_API APlasmaDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	APlasmaDoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlasmaDoor", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent * doorFrame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlasmaDoor", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent * doorPlasmaComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlasmaDoor", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent * doorPlasmaEndComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlasmaDoor", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* plasmaLoopEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlasmaDoor", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem * plasmaEndEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlasmaDoor", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* doorBlocker;

	void StartPlasmaBlocking();
	void StopPlasmaBlocking();
	FORCEINLINE bool GetActiveState() { return bIsActive; }

protected:
	virtual void BeginPlay() override;

private:
	bool bIsActive;
};
