// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Engine/DataTable.h"
#include "SubtitleData.generated.h"

USTRUCT(BlueprintType)
struct FSubtitleData : public FTableRowBase
{
	GENERATED_BODY()

	public:
		FSubtitleData() : time(0.0f) {}

	// Code of this data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString subtitleCode;

	// Subtitle Content
	// & will be replace to enter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString subtitle;

	// Time to show subtitle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float time;

	// Sound code (Not Used)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString soundCode;
};