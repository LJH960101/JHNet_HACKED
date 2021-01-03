#include "Barrier.h"
#include "Engine/Classes/Materials/MaterialInstance.h"
#include "HACKED_CRUSHER.h"

ABarrier::ABarrier()
{
	PrimaryActorTick.bCanEverTick = false;

	lifeTime = 6.0f;
	barrierSize = 3500.0f;

	generator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrier Generator SM"));
	collision = CreateDefaultSubobject<USphereComponent>(TEXT("Barrier Collision"));
	barrierEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Barrier Effect"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_GENERATOR(TEXT("StaticMesh'/Game/Resources/Background/FBX/Gimmick/SM_Mine_Gimmick_001.SM_Mine_Gimmick_001'"));
	if (SM_GENERATOR.Succeeded())
	{
		generator->SetStaticMesh(SM_GENERATOR.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_BARRIER(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P0919_Shield_01.P0919_Shield_01"));
	if (PS_BARRIER.Succeeded())
	{
		barrierEffectComp->SetTemplate(PS_BARRIER.Object);
	}

	RootComponent = generator;
	collision->SetupAttachment(generator);
	barrierEffectComp->SetupAttachment(collision);

	//SET Collision Profile Name
	collision->SetCollisionProfileName("Barrier");
	collision->OnComponentHit.AddDynamic(this, &ABarrier::OnBarrierHit);
	collision->InitSphereRadius(barrierSize);
	barrierEffectComp->SetRelativeLocation(FVector(0.0f, 0.0f, -150.0f));
	barrierEffectComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	generator->SetCollisionProfileName("NoCollision");
	generator->SetWorldScale3D(FVector(0.2f));
}

void ABarrier::BeginPlay()
{
	Super::BeginPlay();
	
	
	GetWorldTimerManager().SetTimer(timer, this, &ABarrier::StopBarrier, lifeTime, false);
}

void ABarrier::StopBarrier()
{
	OnBarrierStop.Broadcast();
	Destroy(true);
}

void ABarrier::OnBarrierHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Barrier Hit!"));
}

