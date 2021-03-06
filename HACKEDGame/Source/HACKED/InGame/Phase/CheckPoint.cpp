// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "CheckPoint.h"
#include "../Character/CRUSHER/HACKED_CRUSHER.h"
#include "../Character/ESPER/HACKED_ESPER.h"

ACheckPoint::ACheckPoint()
{
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnOverlapBegin);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ACheckPoint::OnOverlapEnd);
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
}

void ACheckPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AHACKED_CRUSHER::StaticClass()))
	{
		LOG(Warning,"CURSHER ENTERED!");
		bIsCrusher = true;
	}
	else if (OtherActor->IsA(AHACKED_ESPER::StaticClass()))
	{
		LOG(Warning, "ESPER ENTERED!");
		bIsEsper = true;
	}
	else
	{

	}
}

void ACheckPoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AHACKED_CRUSHER::StaticClass()))
	{
		bIsCrusher = false;
	}
	else if (OtherActor->IsA(AHACKED_ESPER::StaticClass()))
	{
		bIsEsper = false;
	}
	else
	{

	}
}

bool ACheckPoint::IsBothEntered()
{
	return bIsCrusher && bIsEsper;
}

bool ACheckPoint::IsCrusherEntered()
{
	return bIsCrusher;
}

bool ACheckPoint::IsEsperEntered()
{
	return bIsEsper;
}

bool ACheckPoint::IsAnyEntered()
{
	return bIsCrusher || bIsEsper;
}

