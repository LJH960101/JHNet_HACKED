// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Components/ActorComponent.h"
#include "InGame/Spawner/WaveData.h"
#include "AIStatComponent.generated.h"

USTRUCT()
struct FHACKED_AIStat : public FTableRowBase 
{
	GENERATED_BODY()

	FHACKED_AIStat() : type(EHackedAIType::MAX), MaxHp(100.0f), MoveSpeed(100.0f), PatrolRange(500.0f), PatrolDetectRange(100.0f), SR_CanAttackRange(100.0f), SR_AttackDamage(10.0f)  {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EHackedAIType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHp = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MoveSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float PatrolRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float PatrolDetectRange = 100.0f;

	// SecurityRobot 공통 관련 수치 MilitaryRobot은 사용하지 않는 수치이므로 0으로 관리하면된다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float SR_CanAttackRange = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float SR_AttackDamage = 10.0f;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKED_API UAIStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIStatComponent();

	UPROPERTY()
	class UDataTable* AIStat = nullptr;

public:
// AI Function 
	FHACKED_AIStat GetAIStat(const EHackedAIType& type);
};
