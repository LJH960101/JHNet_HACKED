// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "EmptyZoneFinder.h"
#include "Components/BoxComponent.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "InGame/Interface/HACKED_Enemy.h"

// Sets default values
AEmptyZoneFinder::AEmptyZoneFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	_bOnFirstCheck = false;
}

void AEmptyZoneFinder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AEmptyZoneFinder::OnBoxBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &AEmptyZoneFinder::OnBoxEndOverlap);
}

void AEmptyZoneFinder::_AIFirstCheck()
{
	_bOnFirstCheck = true;
	TSet<AActor*> alreadyOverlappedActors;
	Box->GetOverlappingActors(alreadyOverlappedActors);
	for (auto i : alreadyOverlappedActors) {
		if (!_overlappedActors.Contains(i) && i->IsA(AHACKEDCharacter::StaticClass()) || i->GetClass()->ImplementsInterface(UHACKED_Enemy::StaticClass())) {
			_AddActor(i);
		}
	}
}

void AEmptyZoneFinder::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!_overlappedActors.Contains(OtherActor) && OtherActor->IsA(AHACKEDCharacter::StaticClass()) || OtherActor->GetClass()->ImplementsInterface(UHACKED_Enemy::StaticClass())) {
		_AddActor(OtherActor);
	}
}

void AEmptyZoneFinder::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (_overlappedActors.Contains(OtherActor)) {
		_overlappedActors.Remove(OtherActor);
	}
}

bool AEmptyZoneFinder::IsEmpty()
{
	if (!_bOnFirstCheck) _AIFirstCheck();
	return _overlappedActors.Num() == 0;
}

TArray<AActor*> AEmptyZoneFinder::GetExistObject()
{
	if (!_bOnFirstCheck) _AIFirstCheck();
	return _overlappedActors;
}

int AEmptyZoneFinder::GetExistObjectNum()
{
	if (!_bOnFirstCheck) _AIFirstCheck();
	return _overlappedActors.Num();
}

void AEmptyZoneFinder::BeginPlay()
{
	Super::BeginPlay();
	_AIFirstCheck();
}

void AEmptyZoneFinder::_AddActor(AActor* actor)
{
	if (!_overlappedActors.Contains(actor)) {
		_overlappedActors.Push(actor);
	}
}