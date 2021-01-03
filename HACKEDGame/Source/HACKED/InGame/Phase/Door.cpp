// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Door.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	bOpen = false;
	bIsFullyMoved = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::Open()
{

}

void ADoor::Close()
{

}

bool ADoor::IsFullyMoved()
{
	return bIsFullyMoved;
}

void ADoor::LetOpen()
{

}

void ADoor::LetClose()
{

}
