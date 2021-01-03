// Copyright??019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "FadeInOutMaker.h"
#include "InGame/UI/FadeScreen.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AFadeInOutMaker::AFadeInOutMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UFadeScreen> fadeScreenWidget(TEXT("/Game/Blueprint/UI/InGame/Common/WG_FadeScreen.WG_FadeScreen_C"));
	if (fadeScreenWidget.Succeeded()) {
		WG_FadeScreen_Class = fadeScreenWidget.Class;
	}
}

void AFadeInOutMaker::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	WG_FadeScreen = CreateWidget<UFadeScreen>(GetWorld(), WG_FadeScreen_Class);
	WG_FadeScreen->AddToViewport(100);
}

void AFadeInOutMaker::Destroyed()
{
	Super::Destroyed();
	if(WG_FadeScreen){
 		WG_FadeScreen->RemoveFromParent();
		WG_FadeScreen = nullptr;
	}
}

void AFadeInOutMaker::FadeIn(float time)
{
	if(WG_FadeScreen) WG_FadeScreen->FadeIn(time);
}

void AFadeInOutMaker::FadeOut(float time)
{
	if (WG_FadeScreen) WG_FadeScreen->FadeOut(time);
}

void AFadeInOutMaker::FadeInOut(float time, float delay)
{
	if (WG_FadeScreen) WG_FadeScreen->FadeOut(time);
	_fadeOutTime = time;
    GetWorldTimerManager().SetTimer(FadeOutTimer, this, &AFadeInOutMaker::_FadeOut, time + delay, false);
}

void AFadeInOutMaker::GameIntro()
{
	if (WG_FadeScreen) WG_FadeScreen->GameIntro();
}

void AFadeInOutMaker::GameIntroPause()
{
	if (WG_FadeScreen) WG_FadeScreen->GameIntroPause();
}

void AFadeInOutMaker::GameIntroEnd()
{
	if (WG_FadeScreen) WG_FadeScreen->GameIntroEnd();
}

void AFadeInOutMaker::ShowGameOver()
{
	if (WG_FadeScreen) WG_FadeScreen->ShowGameOver();
}

void AFadeInOutMaker::ShowGameClear()
{
	if (WG_FadeScreen) WG_FadeScreen->ShowEndingCredit();
}

void AFadeInOutMaker::BossProduction()
{
	if (WG_FadeScreen) WG_FadeScreen->BossProduction();
}

void AFadeInOutMaker::BossProductionEnd()
{
	if (WG_FadeScreen) WG_FadeScreen->BossProductionEnd();
}

void AFadeInOutMaker::_FadeOut(){
	if (WG_FadeScreen) WG_FadeScreen->FadeIn(_fadeOutTime);
    GetWorldTimerManager().ClearTimer(FadeOutTimer);
}