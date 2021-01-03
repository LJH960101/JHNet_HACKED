// Fill out your copyright notice in the Description page of Project Settings.


#include "BigDoor.h"

ABigDoor::ABigDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	doorFrame = CreateDefaultSubobject<USceneComponent>(TEXT("DOOR FRAME"));
	doorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR LEFT"));
	doorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR RIGHT"));
	RootComponent = doorFrame;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BIG_DOOR_LEFT(TEXT("StaticMesh'/Game/Resources/Object/BigDoor/SM_BigDoor.SM_BigDoor'"));
	if (SM_BIG_DOOR_LEFT.Succeeded())
	{
		doorLeft->SetStaticMesh(SM_BIG_DOOR_LEFT.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BIG_DOOR_RIGHT(TEXT("StaticMesh'/Game/Resources/Object/BigDoor/SM_BigDoor.SM_BigDoor'"));
	if (SM_BIG_DOOR_RIGHT.Succeeded())
	{
		doorRight->SetStaticMesh(SM_BIG_DOOR_RIGHT.Object);
	}

	doorRight->AddLocalRotation(FRotator(0.0f, 180.0f, 0.0f));
	doorLeft->SetupAttachment(doorFrame);
	doorRight->SetupAttachment(doorFrame);
	bOpen = false;
}

void ABigDoor::BeginPlay()
{
	Super::BeginPlay();
	originalLeft = doorLeft->GetComponentLocation();
	originalRight = doorRight->GetComponentLocation();
}

void ABigDoor::Tick(float DeltaTime)
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

void ABigDoor::Open()
{
	bOpen = true;
	bIsFullyMoved = false;
}

void ABigDoor::Close()
{
	bOpen = false;
	bIsFullyMoved = false;
}

void ABigDoor::LetOpen()
{
	doorLeft->SetWorldLocation(FMath::VInterpConstantTo(doorLeft->GetComponentLocation(),
		originalLeft + doorLeft->GetForwardVector() * -522.0f,
		GetWorld()->GetDeltaSeconds(), 522.0f));
	doorRight->SetWorldLocation(FMath::VInterpConstantTo(doorRight->GetComponentLocation(),
		originalRight + doorRight->GetForwardVector() * -522.0f,
		GetWorld()->GetDeltaSeconds(), 522.0f));

	if (FVector::Distance(originalLeft, doorLeft->GetComponentLocation()) > 520.0f)
	{
		bIsFullyMoved = true;
	}
}

void ABigDoor::LetClose()
{
	doorLeft->SetWorldLocation(FMath::VInterpConstantTo(doorLeft->GetComponentLocation(),
		originalLeft,
		GetWorld()->GetDeltaSeconds(), 522.0f));
	doorRight->SetWorldLocation(FMath::VInterpConstantTo(doorRight->GetComponentLocation(),
		originalRight,
		GetWorld()->GetDeltaSeconds(), 522.0f));
	if (FVector::Distance(originalLeft, doorLeft->GetComponentLocation()) <= 1.0f + KINDA_SMALL_NUMBER)
	{
		bIsFullyMoved = true;
	}
}
