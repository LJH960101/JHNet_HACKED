// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Blueprint/UserWidget.h"
#include "WGCharacterSelect.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UWGCharacterSelect : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Select(bool isEsper, int targetSlot);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UnSelect(bool isEsper, int targetSlot);
	
	UFUNCTION(BlueprintImplementableEvent)
	void EndSelect();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PossessProc();
};
