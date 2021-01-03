// Fill out your copyright notice in the Description page of Project Settings.


#include "ElevatorDoor.h"

// Sets default values
AElevatorDoor::AElevatorDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	elevatorDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR"));
	elevatorBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BASE"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_DOOR(TEXT("StaticMesh'/Game/Resources/Object/ElevatorDoor/SM_door_SM_door_Openw_001.SM_door_SM_door_Openw_001'"));
	if (SM_DOOR.Succeeded())
	{
		elevatorDoor->SetStaticMesh(SM_DOOR.Object);
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BASE(TEXT("StaticMesh'/Game/Resources/Object/ElevatorDoor/SM_door_SM_door_Base_001.SM_door_SM_door_Base_001'"));
	if (SM_BASE.Succeeded())
	{
		elevatorBase->SetStaticMesh(SM_BASE.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_OPEN(TEXT("SoundWave'/Game/Sound/Object/ElevatorDoor/Object_Elevator_Open_01.Object_Elevator_Open_01'"));
	if (SB_OPEN.Succeeded())
	{
		openSound = SB_OPEN.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>SB_MOVE(TEXT("SoundWave'/Game/Sound/Object/ElevatorDoor/Object_Elevator_Move_01.Object_Elevator_Move_01'"));
	if (SB_MOVE.Succeeded())
	{
		moveSound = SB_MOVE.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase>SB_CLOSE(TEXT("SoundWave'/Game/Sound/Object/ElevatorDoor/Object_Elevator_Close_01.Object_Elevator_Close_01'"));
	if (SB_CLOSE.Succeeded())
	{
		closeSound = SB_CLOSE.Object;
	}

	RootComponent = elevatorBase;
	elevatorDoor->SetupAttachment(elevatorBase);
}

// Called when the game starts or when spawned
void AElevatorDoor::BeginPlay()
{
	Super::BeginPlay();
	
	//setup target
	target = elevatorDoor->GetComponentLocation() + elevatorDoor->GetComponentLocation().UpVector * 350.0f;
}

// Called every frame
void AElevatorDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isMoving)
	{
		if (!soundDoOnce)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), moveSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, attenuation);
			soundDoOnce = true;
		}
		elevatorDoor->SetWorldLocation(FMath::VInterpConstantTo(elevatorDoor->GetComponentLocation(), target, DeltaTime, 140.0f));
	}
}

void AElevatorDoor::OPEN()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), openSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, attenuation);
	isMoving = true;
}
