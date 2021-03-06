// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Battery.h"
#include "../Stat/HpComponent.h"

ABattery::ABattery()
{
	PrimaryActorTick.bCanEverTick = true;
	pedestal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PEDESTAL"));
	top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TOP"));
	bottom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOTTOM"));
	HpComponent = CreateDefaultSubobject<UHpComponent>(TEXT("HpComponent"));
	beamComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BEAM"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_PEDESTAL(TEXT("StaticMesh'/Game/Resources/Object/Battery/SM_Lab_1_1_Battery_Pedestal_001.SM_Lab_1_1_Battery_Pedestal_001'"));
	if (SM_PEDESTAL.Succeeded())
	{
		pedestal->SetStaticMesh(SM_PEDESTAL.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_TOP(TEXT("StaticMesh'/Game/Resources/Object/Battery/SM_Lap_1_1_Prop_001_Battery_Top_001.SM_Lap_1_1_Prop_001_Battery_Top_001'"));
	if (SM_TOP.Succeeded())
	{
		top->SetStaticMesh(SM_TOP.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BOTTOM(TEXT("StaticMesh'/Game/Resources/Object/Battery/SM_Lap_1_1_Prop_001_Battery_Bottom_001.SM_Lap_1_1_Prop_001_Battery_Bottom_001'"));
	if (SM_BOTTOM.Succeeded())
	{
		bottom->SetStaticMesh(SM_BOTTOM.Object);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_END_BEAM(TEXT("ParticleSystem'/Game/Resources/Object/Battery/FX/P_Core_Active.P_Core_Active'"));
	if(PS_END_BEAM.Succeeded())
	{
		beamEndEffect = PS_END_BEAM.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_LOOP_BEAM(TEXT("ParticleSystem'/Game/Resources/Object/Battery/FX/P_Core_Loop.P_Core_Loop'"));
	if (PS_LOOP_BEAM.Succeeded())
	{
		beamLoopEffect = PS_LOOP_BEAM.Object;
		beamComp->SetTemplate(beamLoopEffect);
	}

	RootComponent = pedestal;
	top->SetupAttachment(pedestal);
	bottom->SetupAttachment(pedestal);
	beamComp->SetupAttachment(bottom);

	pedestal->SetWorldScale3D(FVector::OneVector * 2.0f);
	top->SetWorldScale3D(FVector::OneVector * 2.0f);
	bottom->SetWorldScale3D(FVector::OneVector * 2.0f);
	beamComp->SetWorldScale3D(FVector::OneVector * 0.5f);
	top->AddLocalOffset(FVector::UpVector * 250.0f);
	bottom->AddLocalOffset(FVector::UpVector * 13.5f);
	beamComp->AddLocalOffset(FVector::UpVector * 165.0f);
	beamComp->bAutoActivate = true;
	//beamComp->AddLocalRotation(FRotator(-90.0f,0.0f,0.0f));
	beamComp->Activate();
	isAlive = true;
	doOnce = false;
}

void ABattery::BeginPlay()
{
	Super::BeginPlay();
	HpComponent->SetMaxHP(300.0f);
}

void ABattery::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	originalTopPos = top->GetComponentLocation();
	HpComponent->OnDie.AddUObject(this, &ABattery::Die);
}

void ABattery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isAlive)
	{
		TopFloating();
	}
	else
	{
		Dying();
	}
}

void ABattery::TopFloating()
{
	FVector NewLocation = top->GetComponentLocation();
	float DeltaHeight = (FMath::Sin(stackTime + GetWorld()->GetDeltaSeconds()) - FMath::Sin(stackTime));
	NewLocation.Z += DeltaHeight * floatingHeight;
	stackTime += GetWorld()->GetDeltaSeconds();
	top->SetWorldLocation(NewLocation);
}

void ABattery::Die()
{
	isAlive = false;
	beamComp->Deactivate();
}

void ABattery::Dying()
{
	if (!isAlive)
	{
		top->SetWorldLocation(FMath::VInterpConstantTo(top->GetComponentLocation(),
			originalTopPos + (originalTopPos.UpVector * -1) * 250.0f, GetWorld()->GetDeltaSeconds(), 700.0f));

		if (!doOnce)
		{
			beamComp->SetTemplate(beamEndEffect);
			beamComp->Activate();
			doOnce = true;
		}
		else
		{
			beamComp->Deactivate();
		}

	}
}

float ABattery::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float hp = HpComponent->TakeDamage(DamageAmount);
	return hp;
}


