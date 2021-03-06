// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "NetIDViewer.h"
#include "InGame/Network/Component/NetworkBaseCP.h"

// Sets default values for this component's properties
UNetIDViewer::UNetIDViewer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNetIDViewer::BeginPlay()
{
	Super::BeginPlay();
	textRenderer = NewObject<UTextRenderComponent>(GetOwner());
	textRenderer->RegisterComponent();
	
	if (textRenderer) {
		textRenderer->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		textRenderer->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
		textRenderer->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
		textRenderer->WorldSize = 100.0f;
		textRenderer->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	}

	UActorComponent* cp = GetOwner()->GetComponentByClass(UNetworkBaseCP::StaticClass());
	if (cp) {
		netbaseCp = Cast<UNetworkBaseCP>(cp);
	}
}


// Called every frame
void UNetIDViewer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (textRenderer && netbaseCp) {
		textRenderer->SetText(FText::FromString(netbaseCp->GetObjectIDByString() + "\n" + GetOwner()->GetName()));
	}
}

