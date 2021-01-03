// Fill out your copyright notice in the Description page of Project Settings.


#include "SmallDoor.h"

ASmallDoor::ASmallDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	doorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR FRAME"));
	doorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR LEFT"));
	doorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR RIGHT"));
	RootComponent = doorFrame;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_DOOR_FRAME(TEXT("StaticMesh'/Game/Resources/Object/Door/SM_Door_001.SM_Door_001'"));
	if (SM_DOOR_FRAME.Succeeded())
	{
		doorFrame->SetStaticMesh(SM_DOOR_FRAME.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_DOOR_LEFT(TEXT("StaticMesh'/Game/Resources/Object/Door/SM_Door_Open_001_SM_Open_Door_Left_001.SM_Door_Open_001_SM_Open_Door_Left_001'"));
	if (SM_DOOR_LEFT.Succeeded())
	{
		doorLeft->SetStaticMesh(SM_DOOR_LEFT.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_DOOR_RIGHT(TEXT("StaticMesh'/Game/Resources/Object/Door/SM_Door_Open_001_SM_Open_Door_Right_001.SM_Door_Open_001_SM_Open_Door_Right_001'"));
	if (SM_DOOR_RIGHT.Succeeded())
	{
		doorRight->SetStaticMesh(SM_DOOR_RIGHT.Object);
	}
	doorLeft->SetupAttachment(doorFrame);
	doorRight->SetupAttachment(doorFrame);
	bOpen = false;

}

void ASmallDoor::BeginPlay()
{
	Super::BeginPlay();
	originalLeft = doorLeft->GetComponentLocation();
	originalRight = doorRight->GetComponentLocation();
}

void ASmallDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bOpen)
	{
		LetOpen();
	}
	else
	{
		LetClose();
	}
}

void ASmallDoor::Open()
{
	bOpen = true;
	bIsFullyMoved = false;
}

void ASmallDoor::Close()
{
	bOpen = false;
	bIsFullyMoved = false;
}

void ASmallDoor::LetOpen()
{
	doorLeft->SetWorldLocation(FMath::VInterpConstantTo(doorLeft->GetComponentLocation(),
		originalLeft + doorLeft->GetRightVector() * 120.0f,
		GetWorld()->GetDeltaSeconds(), 120.0f));
	doorRight->SetWorldLocation(FMath::VInterpConstantTo(doorRight->GetComponentLocation(),
		originalRight + doorRight->GetRightVector() * -120.0f,
		GetWorld()->GetDeltaSeconds(), 120.0f));

	if (FVector::Distance(originalLeft, doorLeft->GetComponentLocation()) > 118.0f)
	{
		bIsFullyMoved = true;
	}
}

void ASmallDoor::LetClose()
{
	doorLeft->SetWorldLocation(FMath::VInterpConstantTo(doorLeft->GetComponentLocation(),
		originalLeft,
		GetWorld()->GetDeltaSeconds(), 120.0f));
	doorRight->SetWorldLocation(FMath::VInterpConstantTo(doorRight->GetComponentLocation(),
		originalRight,
		GetWorld()->GetDeltaSeconds(), 120.0f));
	if (FVector::Distance(originalLeft, doorLeft->GetComponentLocation()) <= 1.0f + KINDA_SMALL_NUMBER)
	{
		bIsFullyMoved = true;
	}
}
