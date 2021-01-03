// Fill out your copyright notice in the Description page of Project Settings.


#include "MilitaryNamedAI.h"
#include "InGame/AI/HACKED_AI.h"
#include "PhotonShot.h"
#include "Kismet/KismetMathLibrary.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "MN_AI_AnimInstance.h"
#include "../../Stat/HpComponent.h"
#include "../SMR_AIController.h"


// Sets default values
AMilitaryNamedAI::AMilitaryNamedAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(75.0f, 108.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MNAI_SkeletalMesh(TEXT("/Game/Resources/Enemy/AI/MilitaryNamedAI/MilitaryNamedAI_SK.MilitaryNamedAI_SK"));
	if (MNAI_SkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MNAI_SkeletalMesh.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> MNAI_AnimationBP(TEXT("/Game/Resources/Enemy/AI/MilitaryNamedAI/MilitaryNamedAI_AnimationBP.MilitaryNamedAI_AnimationBP_C"));
	if (MNAI_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MNAI_AnimationBP.Class);
	}

	static ConstructorHelpers::FClassFinder<APhotonShot> StaticPhotonShotClass(TEXT("/Game/Blueprint/InGame/AI/BP_PhotonShot.BP_PhotonShot_C"));
	if (StaticPhotonShotClass.Succeeded()) 
	{
		PhotonCanon = StaticPhotonShotClass.Class;
	}

	AIControllerClass = ASMR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// BT 관련 사거리 
	RushRange = 4000.0f;
	PhotonShotRange = 3000.0f;
	SwapStateRange = 1000.0f;
	RagingBlowRange = 400.0f;

	// 데미지 
	_photonShotSpeed = 3000.0f;
	_photonShotExplodeDamage = 50.0f;
	_photonShotExplodeRange = 300.0f;
	_ragingBlowkDamage = 30.0f;
}

void AMilitaryNamedAI::StateReset()
{
	MN_AI_Anim->AnimIsPhotonShot = false;
	bIsPhotonShot = false;
	MN_AI_Anim->AnimIsRagingBlow = false;
	bIsRagingBlow = false;
	MN_AI_Anim->StopMontage();
}

void AMilitaryNamedAI::OnEnableAI(bool isEnable)
{
	Super::OnEnableAI(isEnable);
	StateReset();
}

void AMilitaryNamedAI::DieProcess()
{
	StateReset();
	Super::DieProcess();
}

void AMilitaryNamedAI::BeHitAnimation()
{
	if (!bCanRigidity || !bCanBeHitAnimation || MN_AI_Anim->AnimIsBeHit) return;
	StateReset();
	MN_AI_Anim->AnimIsBeHit = true;
	LOG_SCREEN("Be Hit");
	Super::BeHitAnimation();
}

void AMilitaryNamedAI::OnEndHitAnimation()
{
	MN_AI_Anim->AnimIsBeHit = false;
	Super::OnEndHitAnimation();
}

void AMilitaryNamedAI::RigidityAI()
{
	if (!bCanRigidity || MN_AI_Anim->AnimIsRigidity) return;
	StateReset();
	MN_AI_Anim->AnimIsRigidity = true;

	Super::RigidityAI();
}

void AMilitaryNamedAI::RigidityEnd()
{
	MN_AI_Anim->AnimIsRigidity = false;
	Super::RigidityEnd();
}

// Called when the game starts or when spawned
void AMilitaryNamedAI::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
	HpComponent->SetMaxHP(GetMaxHp());

	BindRPCFunction(NetBaseCP, AMilitaryNamedAI, PhotonShot);
	BindRPCFunction(NetBaseCP, AMilitaryNamedAI, RagingBlowFirstRide);
	BindRPCFunction(NetBaseCP, AMilitaryNamedAI, RagingBlowSecondRide);
}

void AMilitaryNamedAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitAIStatByType(EHackedAIType::MN_AI);
	MN_AI_Anim = Cast<UMN_AI_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != MN_AI_Anim);
	MN_AI_Anim->OnPhotonShotCheck.AddUObject(this, &AMilitaryNamedAI::PhotonShotSpawn);
	MN_AI_Anim->OnPhotonShotEnd.AddUObject(this, &AMilitaryNamedAI::PhotonShotEnd);
	MN_AI_Anim->OnRagingBlowCheck.AddUObject(this, &AMilitaryNamedAI::RagingBlowDamaging);
	MN_AI_Anim->OnMontageEnded.AddDynamic(this, &AMilitaryNamedAI::OnRagingBlowMontageEnded);
	MN_AI_Anim->OnMNAIRigidityEnd.AddUObject(this, &AMilitaryNamedAI::RigidityEnd);
	MN_AI_Anim->OnMNAIBeHitEnd.AddUObject(this, &AMilitaryNamedAI::OnEndHitAnimation);


}

// Called every frame
void AMilitaryNamedAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AMilitaryNamedAI::RagingBlow(UBehaviorTreeComponent* OwnerComp)
{
	if (bIsRagingBlow || MN_AI_Anim->AnimIsRigidity) return;
	savedCp = OwnerComp;
	if (FMath::RandBool())
	{
		RagingBlowFirstRide();
	}
	else
	{
		RagingBlowSecondRide();
	}
}

void AMilitaryNamedAI::RagingBlowFirstRide()
{
	RPC(NetBaseCP, AMilitaryNamedAI, RagingBlowFirstRide, ENetRPCType::MULTICAST, true);
	currentMontage = 1;
	MN_AI_Anim->PlayRagingBlowMontage();
	MN_AI_Anim->JumpToAttackMontageSection(currentMontage);
	bIsRagingBlow = true;
}

void AMilitaryNamedAI::RagingBlowSecondRide()
{
	RPC(NetBaseCP, AMilitaryNamedAI, RagingBlowSecondRide, ENetRPCType::MULTICAST, true);
	currentMontage = 2;
	MN_AI_Anim->PlayRagingBlowMontage();
	MN_AI_Anim->JumpToAttackMontageSection(currentMontage);
	bIsRagingBlow = true;
}

void AMilitaryNamedAI::RagingBlowNextRide()
{
	if (currentMontage == 1)
	{
		RagingBlowSecondRide();
	}
	else
	{
		RagingBlowFirstRide();
	}
}

bool AMilitaryNamedAI::OnRagingBlow()
{
	return bIsRagingBlow;
}

void AMilitaryNamedAI::OnRagingBlowMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) return;
	if (!bIsRagingBlow) return;
	bIsRagingBlow = false;
	// 존재하면 실행
	if (savedCp) OnRagingBlowEnded.ExecuteIfBound(savedCp);
}

void AMilitaryNamedAI::PhotonShot()
{
	if (NetBaseCP->IsMaster() && !MN_AI_Anim->AnimIsRigidity) {
		RPC(NetBaseCP, AMilitaryNamedAI, PhotonShot, ENetRPCType::MULTICAST, true);
	}
	if (IsDie()) return;
	bIsPhotonShot = true;
	MN_AI_Anim->AnimIsPhotonShot = true;
}

void AMilitaryNamedAI::PhotonShotSpawn()
{
	if (IsDie()) return;
	FVector PhotonShotStartVec = GetMesh()->GetSocketLocation("Bip001-R-Hand");
	GetNearestPlayer(); // Get NearestPlayer를 갱신한다. Slave는 BT가 돌지않아 강제로 해줘야한다.
	NearestPlayerPos.Z -= 80.0f;
	FVector TargetPos = NearestPlayerPos;
	FVector TargetingPos = TargetPos - PhotonShotStartVec;
	TargetingPos.Normalize();
	FVector PhotonShotEndVec = PhotonShotStartVec + TargetingPos * 10000.0f;

	FHitResult Hit(ForceInit);

	FCollisionQueryParams PhotonShotQueryParams(NAME_None, true, this);
	PhotonShotQueryParams.AddIgnoredActor(this);
	//DrawDebugLine(GetWorld(), PhotonShotStartVec, PhotonShotEndVec, FColor::Green, false, 2.f, false, 4.f);

	bool bResult = GetWorld()->LineTraceSingleByChannel(Hit, PhotonShotStartVec, PhotonShotEndVec, ECC_GameTraceChannel10, PhotonShotQueryParams);

	APhotonShot* PhotonShotActor;

	if (Hit.GetActor())
	{
		FVector TargetLocation = Hit.Location;
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
		PhotonShotActor = GetWorld()->SpawnActor<APhotonShot>(PhotonCanon, PhotonShotStartVec, TargetRotation);
	}
	else
	{
		FVector TargetLocation = PhotonShotEndVec;
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
		PhotonShotActor = GetWorld()->SpawnActor<APhotonShot>(PhotonCanon, PhotonShotStartVec, TargetRotation);
	}

	if (!PhotonShotActor) return;
	PhotonShotActor->SetOwnerToMN(this);
	PhotonShotActor->SetSpeed(_photonShotSpeed);
	PhotonShotActor->SetPhotonShotDamage(_photonShotExplodeDamage);
	PhotonShotActor->SetExplosionRange(_photonShotExplodeRange);
	GetMesh()->IgnoreActorWhenMoving(PhotonShotActor, true);
	GetCapsuleComponent()->IgnoreActorWhenMoving(PhotonShotActor, true);

}

void AMilitaryNamedAI::PhotonShotEnd()
{
	bOnEndPhotonShot = true;
	bIsPhotonShot = false;
	MN_AI_Anim->AnimIsPhotonShot = false;
}

void AMilitaryNamedAI::RagingBlowDamaging()
{
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * RagingBlowRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(100.0f),
		Params);

	TArray<AActor*> hitActors;
	for (auto Hit : HitResult)
	{
		if (Hit.GetActor() && !hitActors.Contains(Hit.GetActor()))
		{
			AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(Hit.GetActor());
			if (!hackedPC) continue;
			FDamageEvent DamageEvent;
			hackedPC->TakeDamage(_ragingBlowkDamage, DamageEvent, GetController(), this);
			hitActors.Add(Hit.GetActor());
		}
	}
}



