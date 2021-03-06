// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "HACKED_SkillSystemComponent.h"

// Sets default values for this component's properties
UHACKED_SkillSystemComponent::UHACKED_SkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UDataTable> SPCOST_STAT_DATA(TEXT("/Game/Data/DT_SkillSystem.DT_SkillSystem"));
	CHECK(SPCOST_STAT_DATA.Succeeded());
	SpCostStat = SPCOST_STAT_DATA.Object;
	CHECK(SpCostStat->GetRowMap().Num() > 0);

	// ...
}


FHACKED_SkillSystem_Stat* UHACKED_SkillSystemComponent::GetSpCostStat(const EHackedHeroType& type)
{
	if (SpCostStat == nullptr)
	{
		LOG(Error, "Can't find SpSytem Stat dt");
		return nullptr;
	}

	int rowNum = SpCostStat->GetRowMap().Num();
	for (int i = 1; i <= rowNum; ++i)
	{
		FHACKED_SkillSystem_Stat* statData = SpCostStat->FindRow<FHACKED_SkillSystem_Stat>(*FString::FromInt(i), TEXT(""));
		if (statData == nullptr)
		{
			LOG_S(Error);
			continue;
		}
		if (statData->type == type)
		{
			return statData;
		}
	}

	LOG(Error, "Can't find Hero type in datatable");
	return nullptr;
}
