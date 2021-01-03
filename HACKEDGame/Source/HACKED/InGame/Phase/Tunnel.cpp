// Fill out your copyright notice in the Description page of Project Settings.


#include "Tunnel.h"

ATunnel::ATunnel()
{
 	PrimaryActorTick.bCanEverTick = true;

	tunnel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TUNNEL"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_TUNNEL(TEXT("StaticMesh'/Game/Resources/Background/FBX/SM_Lap_1_1_Prop_Gate_001.SM_Lap_1_1_Prop_Gate_001'"));
	if (SM_TUNNEL.Succeeded())
	{
		tunnel->SetStaticMesh(SM_TUNNEL.Object);
	}
}

void ATunnel::OutlineOn()
{
	tunnel->SetRenderCustomDepth(true);
}

void ATunnel::OutlineOff()
{
	tunnel->SetRenderCustomDepth(false);
}

void ATunnel::BeginPlay()
{
	Super::BeginPlay();
	
	OutlineOff();
}

void ATunnel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

