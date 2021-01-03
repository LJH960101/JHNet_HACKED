// Fill out your copyright notice in the Description page of Project Settings.

#include "SubtitleMaker.h"
#include "InGame/UI/WGSubtitle.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ASubtitleMaker::ASubtitleMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 위젯 클래스 초기화
	static ConstructorHelpers::FClassFinder<UWGSubtitle> subtitleWidget(TEXT("WidgetBlueprint'/Game/Blueprint/UI/InGame/Common/WG_Subtitle.WG_Subtitle_C'"));
	if (subtitleWidget.Succeeded()) {
		WG_Subtitle_Class = subtitleWidget.Class;
	}

	// 데이터 테이블 초기화
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_SUBTITLE_DATA(TEXT("DataTable'/Game/Data/DT_Subtitle.DT_Subtitle'"));
	CHECK(DT_SUBTITLE_DATA.Succeeded());
	_subtitleDataTable = DT_SUBTITLE_DATA.Object;
	CHECK(_subtitleDataTable->GetRowMap().Num() > 0);
}

void ASubtitleMaker::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WG_Subtitle = CreateWidget<UWGSubtitle>(GetWorld(), WG_Subtitle_Class);
	WG_Subtitle->AddToViewport(1);
}

void ASubtitleMaker::Destroyed()
{
	Super::Destroyed();
	if (WG_Subtitle) {
		WG_Subtitle->RemoveFromParent();
		WG_Subtitle = nullptr;
	}
}

void ASubtitleMaker::ShowSubtitleByCode(FString subtitleCode)
{
	CHECK(_subtitleDataTable);
	int rowNum = _subtitleDataTable->GetRowMap().Num();
	for (int i = 1; i <= rowNum; ++i) {
		FSubtitleData* subtitleData = _subtitleDataTable->FindRow<FSubtitleData>(*FString::FromInt(i), TEXT(""));
		if (subtitleData == nullptr) {
			LOG_S(Error);
			continue;
		}
		if(subtitleData->subtitleCode == subtitleCode) {
			WG_Subtitle->ShowSubtitle(subtitleData->subtitle, subtitleData->time);
			return;
		}
	}
	WG_Subtitle->ShowSubtitle(subtitleCode + TEXT(" :: Not exist subtitle code!!"), 5.0f);
}

void ASubtitleMaker::ShowSubtitle(FString subtitle, float time)
{
	WG_Subtitle->ShowSubtitle(subtitle, time);
}

void ASubtitleMaker::StopSubtitle()
{
	WG_Subtitle->StopSubtitle();
}

