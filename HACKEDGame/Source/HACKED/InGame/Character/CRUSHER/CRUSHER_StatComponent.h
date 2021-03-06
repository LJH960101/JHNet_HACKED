// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Components/ActorComponent.h"
#include "CRUSHER_StatComponent.generated.h"

USTRUCT()
struct FHACKED_CRUSHER_Stat : public FTableRowBase
{
	GENERATED_BODY()

	FHACKED_CRUSHER_Stat() : maxHp(300.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float maxHp = 300.0f;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKED_API UCRUSHER_StatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCRUSHER_StatComponent();

	UPROPERTY()
		class UDataTable* CrusherStat = nullptr;
//
//
public:
	FHACKED_CRUSHER_Stat GetCrusherStat();
		
};
