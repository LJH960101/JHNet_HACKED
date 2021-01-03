// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreBattery.h"
#include "InGame/Character/HACKEDCharacter.h"

ACoreBattery::ACoreBattery()
{
	PrimaryActorTick.bCanEverTick = true;
	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("COLLISION"));
	base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BASE"));
	top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TOP"));
	wandukong = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WANDUKONG"));
	coreEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	RootComponent = base;
	collision->InitBoxExtent(FVector(150.0f,200.0f,100.0f));
	collision->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	top->SetupAttachment(base);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BASE(TEXT("StaticMesh'/Game/Resources/Background/FBX/Forest/SM_Prop_005_Base_001.SM_Prop_005_Base_001'"));
	if (SM_BASE.Succeeded())
	{
		base->SetStaticMesh(SM_BASE.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_TOP(TEXT("StaticMesh'/Game/Resources/Background/FBX/Forest/SM_Prop_005_Base_cover_Move_001.SM_Prop_005_Base_cover_Move_001'"));
	if (SM_TOP.Succeeded())
	{
		top->SetStaticMesh(SM_TOP.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_WANDUKONG(TEXT("StaticMesh'/Game/Resources/Background/FBX/Forest/SM_Core_001.SM_Core_001'"));
	if (SM_WANDUKONG.Succeeded())
	{
		wandukong->SetStaticMesh(SM_WANDUKONG.Object);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_CORE(TEXT("ParticleSystem'/Game/Resources/Object/Battery/FX/P_Core_Loop.P_Core_Loop'"));
	if (PS_CORE.Succeeded())
	{
		coreEffectComp->SetTemplate(PS_CORE.Object);
	}
	top->SetupAttachment(base);
	wandukong->SetupAttachment(base);
	collision->SetupAttachment(base);
	coreEffectComp->SetupAttachment(wandukong);
	wandukong->SetRelativeLocation(FVector(0.4953613f, -49.3582001f, 27.9490356f));
	wandukong->SetWorldRotation(FRotator(0.0f, 0.0f, 90.0f));
	wandukong->SetWorldScale3D(FVector(1.25f, 1.25f, 1.25f));
	top->SetRelativeLocation(FVector(-37.8032227f, 0.0f, 46.5881653f));
	coreEffectComp->SetRelativeLocation(FVector(0.0000549f, 0.0000305f, 43.3019524f));
	coreEffectComp->SetWorldRotation(FRotator(0.8362892f, 3.7581296f, 0.0334616f));
	coreEffectComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	collision->OnComponentBeginOverlap.AddDynamic(this, &ACoreBattery::OnOverlap);
}

void ACoreBattery::BeginPlay()
{
	Super::BeginPlay();
	coreEffectComp->Deactivate();
	coreEffectComp->DeactivateSystem();
	BindRPCFunction(NetBaseCP, ACoreBattery, OPEN);
}

void ACoreBattery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (rot >= 50.0f && !doOnce)
	{
		doOnce = true;
		coreEffectComp->Activate();
		coreEffectComp->ActivateSystem();
	}

	if (bIsOpening&&FMath::IsWithin<float>(rot,0.0f,90.0f))
	{
		rot += (DeltaTime*90.0f)/openingTime;
		top->SetRelativeRotation(FRotator(rot, 0.0f, 0.0f));
	}
	else {
		bIsOpening = false;
		bIsDone = true;
	}
}

void ACoreBattery::OPEN()
{
	RPC(NetBaseCP, ACoreBattery, OPEN, ENetRPCType::MULTICAST, true);
	bIsOpening = true;
	OnStartHacking();
}

void ACoreBattery::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(AHACKEDCharacter::StaticClass()))
		return;

	if (bIsDone)
	{
		wandukong->Deactivate();
		coreEffectComp->Deactivate();
		wandukong->SetVisibility(false);
		coreEffectComp->SetVisibility(false);
	}
}
