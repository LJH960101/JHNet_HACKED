// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticField.h"
#include "Blueprint/UserWidget.h"
#include "../../AI/HACKED_AI.h"
#include "HACKED_CRUSHER.h"

AMagneticField::AMagneticField()
{
 	PrimaryActorTick.bCanEverTick = true;
	magField = CreateDefaultSubobject<USphereComponent>(TEXT("MAGNETIC FIELD"));
	magneticEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	magField->InitSphereRadius(activeDistance);
	magField->bHiddenInGame = false;
	RootComponent = magField;
	magneticEffectComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//Temporary Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_PULL(TEXT("ParticleSystem'/Game/Resources/Character/Common/Hacking/P_Hacking_R.P_Hacking_R'"));
	if (PS_PULL.Succeeded())
	{
		pullEffect = PS_PULL.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_PUSH(TEXT("ParticleSystem'/Game/Resources/Character/Common/Hacking/P_Hacking_B.P_Hacking_B'"));
	if (PS_PUSH.Succeeded())
	{
		pushEffect = PS_PUSH.Object;
	}
	magneticEffectComp->SetTemplate(pullEffect);

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
}

void AMagneticField::BeginPlay()
{
	Super::BeginPlay();

	BindRPCFunction(NetBaseCP, AMagneticField, SetMagneticState);

	//Start Selection Timer
	GetWorldTimerManager().SetTimer(selectTimer, this, &AMagneticField::Cancel, selectTime, false);
	magneticEffectComp->SetRelativeLocation(FVector::ZeroVector
		, false, nullptr, ETeleportType::ResetPhysics);
}

void AMagneticField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(NetBaseCP->IsMaster()) PushOrPull(DeltaTime);
}

void AMagneticField::SetMagneticState(bool _isPushState)
{
	RPC(NetBaseCP, AMagneticField, SetMagneticState, ENetRPCType::MULTICAST, true, _isPushState);
	
	isPushState = _isPushState;

	// 당길떄는 조금 크게
	if (isPushState) magField->SetSphereRadius(activeDistance * 1.5f);
	else magField->SetSphereRadius(activeDistance);
}

void AMagneticField::Cancel()
{
	magneticEffectComp->DeactivateSystem();
	magneticEffectComp->Deactivate();
	OnMagFieldStop.Broadcast();
	Destroy(true);
}

void AMagneticField::PushOrPull(float DeltaTime)
{
	if (isPushState) {
		currentSpeed = FMath::Clamp(currentSpeed + (DeltaTime * magForceSpeed * 3.0f), -magForceSpeed, magForceSpeed);
	}
	else {
		currentSpeed = FMath::Clamp(currentSpeed - (DeltaTime * magForceSpeed * 3.0f), -magForceSpeed, magForceSpeed);
	}
	
	if (magneticEffectComp->Template == pushEffect && !isPushState)
		magneticEffectComp->SetTemplate(pullEffect);
	if (magneticEffectComp->Template == pullEffect && isPushState)
		magneticEffectComp->SetTemplate(pushEffect);
	//SEARCH ENEMIES
	TArray<AActor*> nearActors;
	magField->GetOverlappingActors(nearActors, AHACKED_AI::StaticClass());

	//PUSH OR PULL ENEMIES
	for (AActor* iter : nearActors)
	{
		AHACKED_AI* enemy = Cast<AHACKED_AI>(iter);
		if (!enemy) continue;
		FVector pushVector = (GetActorLocation() - enemy->GetActorLocation()).GetUnsafeNormal();
		//MagneticForceSpeed;
		pushVector *= currentSpeed;
		enemy->GetCharacterMovement()->Velocity = pushVector;
	}
}
