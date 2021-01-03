// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class HACKED_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();

	FVector originalLeft;
	FVector originalRight;
	UPROPERTY(EditAnywhere)
	bool bOpen;
	UPROPERTY(EditAnywhere)
	bool bIsFullyMoved;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Open();
	virtual void Close();
	bool IsFullyMoved();

protected:
	virtual void LetOpen();
	virtual void LetClose();
};
