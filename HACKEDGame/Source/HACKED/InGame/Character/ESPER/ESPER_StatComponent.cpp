// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "ESPER_StatComponent.h"

// Sets default values for this component's properties
UESPER_StatComponent::UESPER_StatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> ESPER_STAT_DATA(TEXT("/Game/Data/DT_EsperStat.DT_EsperStat"));
	CHECK(ESPER_STAT_DATA.Succeeded());
	EsperStat = ESPER_STAT_DATA.Object;
	CHECK(EsperStat->GetRowMap().Num() > 0);
	
}


FHACKED_ESPER_Stat UESPER_StatComponent::GetEsperStat()
{
	if (EsperStat == nullptr)
	{
		LOG(Error, "Can't find esper stat dt");
		return FHACKED_ESPER_Stat();
	}

	int rowNum = EsperStat->GetRowMap().Num();
	if (rowNum < 1)
	{
		LOG(Error, "Can't find any Row");
		return FHACKED_ESPER_Stat();
	}

	FHACKED_ESPER_Stat* statData = EsperStat->FindRow<FHACKED_ESPER_Stat>(*FString(TEXT("Esper")), TEXT(""));
	if (statData == nullptr)
	{
		LOG_S(Error);
		return FHACKED_ESPER_Stat();
	}
	return *statData;
}
