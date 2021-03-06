// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "CRUSHER_StatComponent.h"

// Sets default values for this component's properties
UCRUSHER_StatComponent::UCRUSHER_StatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> CRUSHER_STAT_DATA(TEXT("/Game/Data/DT_CrusherStat.DT_CrusherStat"));
	CHECK(CRUSHER_STAT_DATA.Succeeded());
	CrusherStat = CRUSHER_STAT_DATA.Object;
	CHECK(CrusherStat->GetRowMap().Num() > 0);

}



FHACKED_CRUSHER_Stat UCRUSHER_StatComponent::GetCrusherStat()
{
	if (CrusherStat == nullptr)
	{
		LOG(Error, "Can't find crusher stat dt");
		return FHACKED_CRUSHER_Stat();
	}

	int rowNum = CrusherStat->GetRowMap().Num();
	if (rowNum < 1)
	{
		LOG(Error, "Can't find any Row");
		return FHACKED_CRUSHER_Stat();
	}

	FHACKED_CRUSHER_Stat* statData = CrusherStat->FindRow<FHACKED_CRUSHER_Stat>(*FString(TEXT("Crusher")), TEXT(""));
	if (statData == nullptr)
	{
		LOG_S(Error);
		return FHACKED_CRUSHER_Stat();
	}
	return *statData;
}
