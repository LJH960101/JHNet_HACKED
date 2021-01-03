// Fill out your copyright notice in the Description page of Project Settings.

#include "SelfDestructAI.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "SD_AI_AnimInstance.h"
#include "../../Stat/HpComponent.h"
#include "../SMR_AIController.h"

// Sets default values
ASelfDestructAI::ASelfDestructAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(45.0f, 100.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SD_AI_SkeletalMesh(TEXT("/Game/Resources/Enemy/AI/SelfDestruct_AI/SelfDestruct_SK.SelfDestruct_SK"));
	if (SD_AI_SkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SD_AI_SkeletalMesh.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> SD_AI_AnimationBP(TEXT("/Game/Resources/Enemy/AI/SelfDestruct_AI/SelfDestructAI_AnimBP.SelfDestructAI_AnimBP_C"));
	if (SD_AI_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SD_AI_AnimationBP.Class);
	}

	AIControllerClass = ASMR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	_destructDamage = 100.0f;
	_destructRange = 350.0f;
}

void ASelfDestructAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitAIStatByType(EHackedAIType::SD_AI);
	SD_AI_Anim = Cast<USD_AI_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != SD_AI_Anim);

	SD_AI_Anim->OnSelfDestructDamaging.AddUObject(this, &ASelfDestructAI::SelfDestructDamaging);
}

void ASelfDestructAI::OnSpawn()
{
	Super::OnSpawn();
	ResetAI();
}

// Called when the game starts or when spawned
void ASelfDestructAI::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
	HpComponent->SetMaxHP(GetMaxHp());
	
	BindRPCFunction(NetBaseCP, ASelfDestructAI, SelfDestruct);
}

void ASelfDestructAI::DieProcess()
{
	Super::DieProcess();
	ResetAI();
}

// Called every frame
void ASelfDestructAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASelfDestructAI::SelfDestruct()
{
	RPC(NetBaseCP, ASelfDestructAI, SelfDestruct, ENetRPCType::MULTICAST, true);
	LOG_SCREEN("1");
	bIsSelfDestruct = true;
	SD_AI_Anim->AnimIsSelfDestruct = true;
}

void ASelfDestructAI::SelfDestructDamaging()
{
	LOG_SCREEN("2");
	TArray<FHitResult> DetectResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		DetectResult,
		GetActorLocation(),
		GetActorLocation() + FVector(0.0f, 0.0f, 0.01f),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(_destructRange),
		Params);

	if (bResult)
	{
		for (auto Hit : DetectResult)
		{
			if (Hit.GetActor())
			{
				AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(Hit.GetActor());
				if (!hackedPC) continue;
				FDamageEvent DamageEvent;
				hackedPC->TakeDamage(_destructDamage, DamageEvent, GetController(), this);				
			}
		}
		GetWorldTimerManager().SetTimer(SelfDestructTimerHandle, this, &ASelfDestructAI::SelfDestructDestroy, 0.3f, false);
	}
}

void ASelfDestructAI::SelfDestructDestroy()
{
	// 강제로 죽음을 처리함.
	AIAllStopTool::AIDisable(this);
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	ResetAI();
	HACKEDInGameManager->GetSpawner()->DestroyObject(this);
}

void ASelfDestructAI::ResetAI()
{
	bIsSelfDestruct = false;
	SD_AI_Anim->AnimIsSelfDestruct = false;
	GetWorldTimerManager().ClearTimer(SelfDestructTimerHandle);
}