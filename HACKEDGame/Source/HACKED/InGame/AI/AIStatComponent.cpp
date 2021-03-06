// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "AIStatComponent.h"
#include "HACKED_AI.h"



// AI관련 모든 스텟 관리 클래스 
// Sets default values for this component's properties
UAIStatComponent::UAIStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UDataTable> AI_STAT_DATA(TEXT("/Game/Data/DT_AIStat.DT_AIStat"));
	CHECK(AI_STAT_DATA.Succeeded());
	AIStat = AI_STAT_DATA.Object;
	CHECK(AIStat->GetRowMap().Num() > 0);
}


FHACKED_AIStat UAIStatComponent::GetAIStat(const EHackedAIType& type)
{
	if (AIStat == nullptr) {
		LOG(Error, "Can't find ai stat dt.");
		return FHACKED_AIStat();
	}

	int rowNum = AIStat->GetRowMap().Num();
	for (int i = 1; i <= rowNum; ++i) {
		FHACKED_AIStat* statData = AIStat->FindRow<FHACKED_AIStat>(*FString::FromInt(i), TEXT(""));
		if (statData == nullptr) {
			LOG_S(Error);
			continue;
		}
		if (statData->type == type) {
			return *statData;
		}
	}
	LOG(Error, "Can't find AI type in datatable.");
	return FHACKED_AIStat();
}