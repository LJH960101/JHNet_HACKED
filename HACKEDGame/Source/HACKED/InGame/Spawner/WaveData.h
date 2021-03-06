// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Engine/DataTable.h"
#include "WaveData.generated.h"

// Spawn Object types
UENUM(BlueprintType)
enum class EHackedAIType : uint8
{
	MS_AI		UMETA(DisplayName = "MS_AI"),
	MN_AI		UMETA(DisplayName = "MN_AI"),
	SD_AI		UMETA(DisplayName = "SD_AI"),
	SR_ET		UMETA(DisplayName = "SR_ET"),
	SR_LS		UMETA(DisplayName = "SR_LS"),
	SR_SLS		UMETA(DisplayName = "SR_SLS"),
	MAX
};

UClass* GetAIClassByType(const EHackedAIType& type);

USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FWaveData() : waveNumb(0), aiType(EHackedAIType::MAX), spawnMany(0), spawnPosition(""), spawnGab(0.0f), spawnGabMany(0) {}

	// Number of wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")	
	int32 waveNumb;

	// Type of ai
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EHackedAIType aiType;

	// Total number of ai
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 spawnMany;

	// Spawn target position. (format is "4-5-6" or "")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString spawnPosition;

	// Gab of spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float spawnGab;

	// Number of spawn per gab
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 spawnGabMany;
};