// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

// If u attach to actor, u can see the NetID by textRenderer.
// JHL

#pragma once

#include "HACKED.h"
#include "Components/ActorComponent.h"
#include "NetIDViewer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKED_API UNetIDViewer : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UTextRenderComponent* textRenderer = nullptr;
	class UNetworkBaseCP* netbaseCp = nullptr;
	
public:	
	// Sets default values for this component's properties
	UNetIDViewer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
