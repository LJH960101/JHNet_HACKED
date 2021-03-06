// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "EmptyZoneFinder.generated.h"


/*
	충돌 범위 안에 캐릭터(몬스터, PC)가 존재하는지 판단하는 액터입니다.
*/
UCLASS()
class HACKED_API AEmptyZoneFinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEmptyZoneFinder();
	virtual void PostInitializeComponents() override;

	// 안에 아무도 없는지 받습니다.
	UFUNCTION(BlueprintPure, Category = "Empty Zone")
	bool IsEmpty();

	// 안으로 들어온 오브젝트를 받습니다.
	UFUNCTION(BlueprintCallable, Category = "Empty Zone")
	TArray<AActor*> GetExistObject();

	// 안으로 들어온 오브젝트의 수를 구합니다.
	UFUNCTION(BlueprintPure, Category = "Empty Zone")
	int GetExistObjectNum();

	TArray<AActor*> _overlappedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box;

	virtual void BeginPlay() override;

private:
	bool _bOnFirstCheck;

	// 이미 overlapping된 액터를 조사한다.
	// 여기서 처리를 하지 않으면 시작하기전에 이미 존재하는 액터는 포함되지 않는다.
	UFUNCTION()
	void _AIFirstCheck();

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void _AddActor(AActor* actor);
};
