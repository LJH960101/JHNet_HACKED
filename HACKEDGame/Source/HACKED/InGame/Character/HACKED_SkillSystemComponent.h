// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Components/ActorComponent.h"
#include "HACKED_SkillSystemComponent.generated.h"


UENUM(BlueprintType)
enum class EHackedHeroType : uint8
{
	ESPER		UMETA(DisplayName = "ESPER"),
	CRUSHER		UMETA(DisplayName = "CRUSHER"),
	Max
};

UENUM(BlueprintType)
enum class ESkillNumberType : uint8
{
	M2Skill = 1,
	FirstSkill,
	SecondSkill,
	ThirdSkill,
	DashSkill
};



USTRUCT()
struct FHACKED_SkillSystem_Stat : public FTableRowBase
{
	GENERATED_BODY()

	FHACKED_SkillSystem_Stat() : type(EHackedHeroType::Max), m2SkillTimeCost(3), firstSkillTimeCost(5), secondSkillTimeCost(7), thirdSkillTimeCost(5), dashSkillTimeCost(2), UltimateGaugeCost(300.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EHackedHeroType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int m2SkillTimeCost = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int firstSkillTimeCost = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int secondSkillTimeCost = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int thirdSkillTimeCost = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int dashSkillTimeCost = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float UltimateGaugeCost = 300.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKED_API UHACKED_SkillSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHACKED_SkillSystemComponent();

	UPROPERTY()
	class UDataTable* SpCostStat = nullptr;


public:
	FHACKED_SkillSystem_Stat* GetSpCostStat(const EHackedHeroType& type);

		
};
