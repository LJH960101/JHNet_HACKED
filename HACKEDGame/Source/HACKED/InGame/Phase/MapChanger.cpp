// Fill out your copyright notice in the Description page of Project Settings.

#include "MapChanger.h"
#include "InGame/HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"

// Sets default values
AMapChanger::AMapChanger()
{
 	PrimaryActorTick.bCanEverTick = false;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMapChanger::OverlapBegins);
}

// Called when the game starts or when spawned
void AMapChanger::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(HACKEDInGameManager);
}


void AMapChanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapChanger::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//FLatentActionInfo LatentInfo;
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter /*&& LevelToLoad != ""*/)
	{
		/*LOG_SCREEN("WE'RE HERE TO LOAD");
		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);*/

		// 크러셔/에스퍼 해당 Start Point로 이동시킨다.
		if (OtherActor->IsA(AHACKED_ESPER::StaticClass()) && EsperNewStartPoint) OnEsperEnter();
		else if (OtherActor->IsA(AHACKED_CRUSHER::StaticClass()) && CrusherNewStartPoint) OnCrusherEnter();

		/*OnLoad();*/
	}
	/*if (OtherActor == MyCharacter && LevelToUnload != "")
	{
		LOG_SCREEN("WE'RE HERE TO UNLOAD");
		UGameplayStatics::UnloadStreamLevel(this,LevelToUnload,LatentInfo,true);
	}*/
}

