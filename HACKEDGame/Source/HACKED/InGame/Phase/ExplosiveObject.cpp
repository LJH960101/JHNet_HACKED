// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveObject.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"

AExplosiveObject::AExplosiveObject()
{
	PrimaryActorTick.bCanEverTick = false;

	explosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("COLLISION"));
	explosionDamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DAMAGE"));
	explosionEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));

	collision->SetCollisionProfileName("BlockAll");

	explosionEffectComp->SetAutoActivate(false);

	collision->OnComponentHit.AddDynamic(this, &AExplosiveObject::OnHit);
	explosionEffectComp->OnSystemFinished.AddDynamic(this, &AExplosiveObject::OnFinished);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SM_OBJECT(TEXT("StaticMesh'/Game/Resources/Background/FBX/Forest/ActerGroup/Gimmick_Acter/SM_Missile_Acter_001.SM_Missile_Acter_001'"));
	if (SM_OBJECT.Succeeded())
	{
		explosiveMesh->SetStaticMesh(SM_OBJECT.Object);
	}
		//static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_EXPLOSION(TEXT("ParticleSystem'/Game/Resources/Object/Explosive/P_Explosion.P_Explosion'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_EXPLOSION(TEXT("ParticleSystem'/Game/HACKED_AI/Bomb_Mine/P_Explosion_V4.P_Explosion_V4'"));
	if (PS_EXPLOSION.Succeeded())
	{
		explosionEffectComp->SetTemplate(PS_EXPLOSION.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundWave>SB_BOOM(TEXT("SoundWave'/Game/Sound/Object/ExplosiveObject/TNT_BOOM_01.TNT_BOOM_01'"));
	if (SB_BOOM.Succeeded())
	{
		explosionSound=SB_BOOM.Object;	
	}
	static ConstructorHelpers::FObjectFinder<USoundAttenuation>SA_BOOM(TEXT("SoundAttenuation'/Game/Sound/Attenuations/OBJECT/EXPLOSION.EXPLOSION'"));
	if (SA_BOOM.Succeeded()) { explosionAttenuation = SA_BOOM.Object; }

	explosionDamageSphere->InitSphereRadius(damageRadius);
	RootComponent = explosiveMesh;
	explosionEffectComp->SetupAttachment(explosiveMesh);
	collision->SetupAttachment(explosiveMesh);
	explosionDamageSphere->SetupAttachment(collision);
	collision->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	collision->SetWorldScale3D(FVector(1.75f, 1.75f, 1.75f));
	explosionEffectComp->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));
}

void AExplosiveObject::BeginPlay()
{
	Super::BeginPlay();
	
	explosionEffectComp->Deactivate();
	explosionEffectComp->DeactivateSystem();

	BindHACKEDInGameManager(HACKEDInGameManager);

	BindRPCFunction(NetBaseCP, AExplosiveObject, RPCExplode);
}

void AExplosiveObject::Explode(AActor* explodeBy)
{
	if (!bCanExplode)
		return;
	//if enter this function
	if (doOnce)
		return;

	RPCExplode();
}

void AExplosiveObject::RPCExplode()
{
	RPC(NetBaseCP, AExplosiveObject, RPCExplode, ENetRPCType::MULTICAST, true);

	HACKEDInGameManager->GetQuestManager()->CompleteSubQuest("SQ6");
	doOnce = true;

	explosionEffectComp->Activate();
	explosionEffectComp->ActivateSystem();
	explosiveMesh->SetVisibility(false);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), explosionSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, explosionAttenuation);

	isExploded = true;
	SetOutline(false);
	SetCanExplosive(false);
	SetActorEnableCollision(false);
}

void AExplosiveObject::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	_hitCount++;
	if (_hitCount > 4) {
		Explode(OtherActor);
	}
	
}

void AExplosiveObject::OnFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}

