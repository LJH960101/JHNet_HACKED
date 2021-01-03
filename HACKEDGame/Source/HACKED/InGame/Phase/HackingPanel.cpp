// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "HackingPanel.h"
#include "../Character/HACKEDCharacter.h"
#include "../Character/CRUSHER/HACKED_CRUSHER.h"
#include "../Character/ESPER/HACKED_ESPER.h"

FColor AHackingPanel::ReadyColor = FColor::Red;
FColor AHackingPanel::RunningColor = FColor::Orange;
FColor AHackingPanel::EndColor = FColor::Green;

AHackingPanel::AHackingPanel()
{
	PrimaryActorTick.bCanEverTick = false;

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));

	interactionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("INTERACTION BOX"));
	hackingPanelComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HACKING PANEL"));
	hologramComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HOLOGRAM STATE"));
	TEST_light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));

	RootComponent = interactionBox;
	hackingPanelComp->SetupAttachment(interactionBox);
	hologramComp->SetupAttachment(interactionBox);
	TEST_light->SetupAttachment(interactionBox);

	interactionBox->SetRelativeLocation(FVector::ZeroVector);
	hackingPanelComp->SetRelativeLocation(FVector::ZeroVector);
	hologramComp->SetRelativeLocation(FVector::UpVector*150.0f);
	TEST_light->SetRelativeLocation(FVector::UpVector*150.0f);

	bDoOnceCheck = false;
	bIsRunning = false;
	bIsActive = false;
	bIsPlayerOverlapped = false;

	interactionBox->InitBoxExtent(FVector::OneVector * 300.0f);
	interactionBox->OnComponentBeginOverlap.AddDynamic(this, &AHackingPanel::OnBeginOverlap);
	interactionBox->OnComponentEndOverlap.AddDynamic(this, &AHackingPanel::OnEndOverlap);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_HACKING_PANEL(TEXT("StaticMesh'/Game/Resources/Object/HackingPanel/SM_HackingPanel.SM_HackingPanel'"));
	if (SM_HACKING_PANEL.Succeeded())
	{
		hackingPanelComp->SetStaticMesh(SM_HACKING_PANEL.Object);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_READY(TEXT("ParticleSystem'/Game/Resources/Character/Common/Hacking/P_Hacking_R.P_Hacking_R'"));
	if (PS_READY.Succeeded())
	{
		readyEffect = PS_READY.Object;
		hologramComp->SetTemplate(readyEffect);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_RUNNING(TEXT("ParticleSystem'/Game/Resources/Character/Common/Hacking/P_Hacking_B.P_Hacking_B'"));
	if (PS_RUNNING.Succeeded())
	{
		runningEffect = PS_RUNNING.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_END(TEXT("ParticleSystem'/Game/Resources/Character/Common/Hacking/P_Hacking_G.P_Hacking_G'"));
	if (PS_END.Succeeded())
	{
		endEffect = PS_END.Object;
	}

	TEST_light->SetLightColor(ReadyColor);


	SetHackingState(EHackingState::HACKING_READY);
}

void AHackingPanel::BeginPlay()
{
	Super::BeginPlay();
	BindRPCFunction(NetBaseCP, AHackingPanel, StartHacking);
	hackingPanelComp->bHiddenInGame = true;
	hackingPanelComp->SetStaticMesh(nullptr);
	hologramComp->bHiddenInGame = true;
	hologramComp->Deactivate();
}

//void AHackingPanel::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

void AHackingPanel::StartHacking()
{
	RPC(NetBaseCP, AHackingPanel, StartHacking, ENetRPCType::MULTICAST, true);

	//CHECK ACTIVE OR NOT
	if (!bIsActive)
		return;

	//PREVENT PRESS MORE THAN ONCE
	if (bDoOnceCheck)
		return;

	//CHECK PLAYER OVERLAP
	if (bIsPlayerOverlapped)
	{
		bDoOnceCheck = true;
		bIsRunning = true;
		LOG_SCREEN("HACKING START");
		HackingState();
	}
	
}

void AHackingPanel::HackingState()
{
	SetHackingState(EHackingState::HACKING_RUNNING);
	hologramComp->DeactivateSystem();
	hologramComp->SetTemplate(runningEffect);
	hologramComp->ActivateSystem();
	TEST_light->SetLightColor(RunningColor);
	LOG_SCREEN("HACKING...");
}

void AHackingPanel::EndHacking()
{
	SetHackingState(EHackingState::HACKING_END);
	hologramComp->DeactivateSystem();
	hologramComp->SetTemplate(endEffect);
	hologramComp->ActivateSystem();
	TEST_light->SetLightColor(EndColor);
	bIsRunning = false;
	
	LOG_SCREEN("HACKING COMPLETED!!");
}

void AHackingPanel::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AHACKEDCharacter::StaticClass()))
	{
		AHACKEDCharacter* player = Cast<AHACKEDCharacter>(OtherActor);
		player->SetHackingPanel(this);
		bIsPlayerOverlapped=true;
		LOG_SCREEN("Player In");
	}
}

void AHackingPanel::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AHACKEDCharacter::StaticClass()))
	{
		AHACKEDCharacter* player = Cast<AHACKEDCharacter>(OtherActor);
		player->SetHackingPanel(nullptr);
		bIsPlayerOverlapped = false;
		LOG_SCREEN("Player Out");
	}
}

