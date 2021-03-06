// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Blueprint/UserWidget.h"
#include "FadeScreen.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UFadeScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Fade)
	void FadeIn(float Time);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Fade)
	void FadeOut(float Time);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GameOver)
	void ShowGameOver();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GameIntro)
	void GameIntro();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GameIntro)
	void GameIntroPause();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GameIntro)
	void GameIntroEnd();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = EndingCredit)
	void ShowEndingCredit();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = BossProduction)
	void BossProduction();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = BossProduction)
	void BossProductionEnd();

};
