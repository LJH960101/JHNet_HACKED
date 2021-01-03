// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectroMagneticPulse.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "InGame/Stat/HpComponent.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"

// Sets default values
AElectroMagneticPulse::AElectroMagneticPulse()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ElectroMagnetPulse = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElectroMagnetPulse"));
	HpComponent = CreateDefaultSubobject<UHpComponent>(TEXT("HpComponent"));
	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_EMP(TEXT("/Game/Resources/Enemy/AI/MilitaryBossAI/ElectroMagneticPulse.ElectroMagneticPulse"));
	if (SM_EMP.Succeeded())
	{
		ElectroMagnetPulse->SetStaticMesh(SM_EMP.Object);
	}

	RootComponent = ElectroMagnetPulse;


	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_EMPEXPLOSION(TEXT("/Game/HACKED_AI/Bomb_Mine/P_Elec_Explosion.P_Elec_Explosion"));
	if (PS_EMPEXPLOSION.Succeeded())
		PS_EMPExplosion = PS_EMPEXPLOSION.Object;

}

// Called when the game starts or when spawned
void AElectroMagneticPulse::BeginPlay()
{
	Super::BeginPlay();
	HpComponent->SetMaxHP(200.0f);
	BindRPCFunction(NetBaseCP, AElectroMagneticPulse, EMPShock);
}

void AElectroMagneticPulse::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HpComponent->OnDie.AddUObject(this, &AElectroMagneticPulse::EMPShock);
}

// Called every frame
void AElectroMagneticPulse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AElectroMagneticPulse::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (MilitaryBoss)
	{
		if (!MilitaryBoss->_bIsBossImmortal)
		{
			return HpComponent->GetHP();
		}

	}



	float hp = HpComponent->TakeDamage(DamageAmount);
	return hp;
}

void AElectroMagneticPulse::EMPShock()
{
	RPC(NetBaseCP, AElectroMagneticPulse, EMPShock, ENetRPCType::MULTICAST, true);

	auto psEMP = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_EMPExplosion, GetActorLocation(), GetActorRotation(), true);
	psEMP->SetWorldScale3D(FVector(7.0f, 7.0f, 7.0f));

	// 폭탄과 보스의 거리가 10000 이하라면 무적모드를 해제한다.
	if (FVector::Dist(MilitaryBoss->GetActorLocation(), GetActorLocation()) < 10000.0f) {
		MilitaryBoss->SetImmortalMode(false);
	}

	// 다른 EMP의 아웃라인을 해제한다.
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AElectroMagneticPulse::StaticClass(), outActors);

	for (auto actor : outActors)
	{
		AElectroMagneticPulse* ElectroMagneticPulse = Cast<AElectroMagneticPulse>(actor);
		if (!ElectroMagneticPulse) continue;
		ElectroMagneticPulse->SetOutline(false);
	}

	OnExplosion();
	Destroy();
}

void AElectroMagneticPulse::SetOutline(bool isOutLine)
{
	ElectroMagnetPulse->SetRenderCustomDepth(isOutLine);
}



