// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 


#include "PlasmaDoor.h"

APlasmaDoor::APlasmaDoor()
{
	PrimaryActorTick.bCanEverTick = false;
	doorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR FRAME"));
	doorPlasmaComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DOOR PLASMA COMP"));
	doorPlasmaEndComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DOOR PLASMA END COMP"));
	doorBlocker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR BLOCKER"));

	bIsActive = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_DOOR_FRAME(TEXT("StaticMesh'/Game/Resources/Object/PlasmaDoor/SM_Lab_1_1_Room_Gate_001.SM_Lab_1_1_Room_Gate_001'"));
	if (SM_DOOR_FRAME.Succeeded())
	{
		doorFrame->SetStaticMesh(SM_DOOR_FRAME.Object);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_LOOP_PLASMA(TEXT("ParticleSystem'/Game/Resources/Object/Gate/P_Gate.P_Gate'"));
	if (PS_LOOP_PLASMA.Succeeded())
	{
		plasmaLoopEffect = PS_LOOP_PLASMA.Object;
		doorPlasmaComp->SetTemplate(plasmaLoopEffect);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_END_PLASMA(TEXT("ParticleSystem'/Game/Resources/Object/Gate/P_Gate_Close.P_Gate_Close'"));
	if (PS_END_PLASMA.Succeeded())
	{
		plasmaEndEffect = PS_END_PLASMA.Object;
		doorPlasmaEndComp->SetTemplate(plasmaEndEffect);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_DOOR_BLOCKER(TEXT("StaticMesh'/Game/Resources/Object/PlasmaDoor/DoorBlocker.DoorBlocker'"));
	if (SM_DOOR_BLOCKER.Succeeded())
	{
		doorBlocker->SetStaticMesh(SM_DOOR_BLOCKER.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial>M_TRANSPARENT(TEXT("Material'/Game/Resources/Object/PlasmaDoor/Transparent.Transparent'"));
	if (M_TRANSPARENT.Succeeded())
	{
		doorBlocker->SetMaterial(0, M_TRANSPARENT.Object);
	}


	doorBlocker->SetWorldScale3D(FVector(1.5f,2.0f,1.0f));
	doorBlocker->AddLocalRotation(FRotator(90.0f,0.0f,0.0f));
	doorBlocker->SetCollisionProfileName(FName("PlasmaDoor"));
	
	RootComponent = doorFrame;
	doorPlasmaComp->SetupAttachment(doorFrame);
	doorPlasmaEndComp->SetupAttachment(doorFrame);
	doorBlocker->SetupAttachment(doorPlasmaComp);

	doorPlasmaComp->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));
	doorPlasmaComp->AddLocalOffset(FVector(0.0f,350.0f,210.0f));
	doorPlasmaEndComp->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));
	doorPlasmaEndComp->AddLocalOffset(FVector(0.0f, 350.0f, 210.0f));

	doorPlasmaEndComp->Deactivate();
	doorBlocker->AddLocalOffset(FVector(120.0f,0.0f,0.0f));
}

void APlasmaDoor::StartPlasmaBlocking()
{
	LOG_SCREEN("%s START PLASMA",*GetName());
	doorPlasmaComp->Activate();
	doorPlasmaComp->ActivateSystem();
	doorPlasmaEndComp->Deactivate();
	doorPlasmaEndComp->DeactivateSystem();
	doorBlocker->SetCollisionProfileName(FName("PlasmaDoor"));
	doorBlocker->Activate();
	bIsActive = true;
}

void APlasmaDoor::StopPlasmaBlocking()
{
	LOG_SCREEN("%s STOP PLASMA",*GetName());
	doorPlasmaComp->Deactivate();
	doorPlasmaComp->DeactivateSystem();
	doorPlasmaEndComp->Activate();
	doorPlasmaEndComp->ActivateSystem();
	doorBlocker->SetCollisionProfileName(FName("NoCollision"));
	doorBlocker->Deactivate();
	bIsActive = false;
}

void APlasmaDoor::BeginPlay()
{
	Super::BeginPlay();
}