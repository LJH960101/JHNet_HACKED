// Copyright??019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "FadeInOutMaker.generated.h"

UCLASS()
class HACKED_API AFadeInOutMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFadeInOutMaker();

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeIn(float time);

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeOut(float time);

	// FadeOut 이후 delay 만큼의 시간이 지나면 FadeIn 됨
	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeInOut(float time, float delay);

	UFUNCTION(BlueprintCallable, Category = GameIntro)
	void GameIntro();

	UFUNCTION(BlueprintCallable, Category = GameIntro)
	void GameIntroPause();

	UFUNCTION(BlueprintCallable, Category = GameIntro)
	void GameIntroEnd();

	UFUNCTION(BlueprintCallable, Category = GameOver)
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = GameClear)
	void ShowGameClear();

	UFUNCTION(BlueprintCallable, Category = BossProduction)
	void BossProduction();

	UFUNCTION(BlueprintCallable, Category = BossProduction)
	void BossProductionEnd();
protected:
	// Called when the game starts or when spawned
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

private:
	UPROPERTY()
	TSubclassOf<class UFadeScreen> WG_FadeScreen_Class;
	UPROPERTY()
	class UFadeScreen* WG_FadeScreen = nullptr;

	FTimerHandle FadeOutTimer;

	void _FadeOut();
	float _fadeOutTime;
};
