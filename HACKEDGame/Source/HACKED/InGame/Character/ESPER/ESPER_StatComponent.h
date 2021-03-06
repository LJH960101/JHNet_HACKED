// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Components/ActorComponent.h"
#include "ESPER_StatComponent.generated.h"

USTRUCT()
struct FHACKED_ESPER_Stat : public FTableRowBase
{
	GENERATED_BODY()

	FHACKED_ESPER_Stat() : maxHp(300.0f), primaryAttackDamage(100.0f), paspDamage(100.0f), paspRange(100.0f), paSpeed(3000.0f),
		psychicForceDamage(100.0f), psychicForceRange(1000.0f), psychicDropDamage(100.0f), psychicDropRange(500.0f),
		psychicWaveDamage(20.0f), psychicWaveRange(1000.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float maxHp = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float primaryAttackDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float paspDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float paspRange = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float paSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float psychicForceDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float psychicForceRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float psychicDropDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float psychicDropRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float psychicWaveDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float psychicWaveRange = 1000.0f;

};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKED_API UESPER_StatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UESPER_StatComponent();

	UPROPERTY()
	class UDataTable* EsperStat = nullptr;


public:
	FHACKED_ESPER_Stat GetEsperStat();
		
};
