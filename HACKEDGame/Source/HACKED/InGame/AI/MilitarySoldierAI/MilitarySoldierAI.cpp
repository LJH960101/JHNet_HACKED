// Fill out your copyright notice in the Description page of Project Settings.


#include "MilitarySoldierAI.h"
#include "InGame/AI/HACKED_AI.h"
#include "Kismet/KismetMathLibrary.h"
#include "InGame/Character/CRUSHER/Barrier.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "MS_AI_AnimInstance.h"
#include "../../Stat/HpComponent.h"
#include "../SMR_AIController.h"

// Sets default values
AMilitarySoldierAI::AMilitarySoldierAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(75.0f, 108.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MSAI_SkeletalMesh(TEXT("/Game/Resources/Enemy/AI/MilitarySoldierAI/NEW/AI_Soldier_riggiedMesh.AI_Soldier_riggiedMesh"));
	if (MSAI_SkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MSAI_SkeletalMesh.Object);
	}
	
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> MSAI_AnimationBP(TEXT("/Game/Resources/Enemy/AI/MilitarySoldierAI/NEW/MilitarySoldierAI_AnimationBP.MilitarySoldierAI_AnimationBP_C"));
	if (MSAI_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MSAI_AnimationBP.Class);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MS_LASERSHOCK(TEXT("/Game/HACKED_AI/P_AIBeam.P_AIBeam"));
	{
		PS_LaserShock = MS_LASERSHOCK.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MS_LASERSHOCKHIT(TEXT("/Game/Resources/Enemy/AI/P_AI_Beam_Impact.P_AI_Beam_Impact"));
	{
		PS_LaserShockHit = MS_LASERSHOCKHIT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MS_BLADESHOCKHIT(TEXT("/Game/Resources/Enemy/AI/P_AI_BladeHit.P_AI_BladeHit"));
	{
		PS_BladeShockHit = MS_BLADESHOCKHIT.Object;
	}


	AIControllerClass = ASMR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	_laserAttackDamage = 20.0f;
	_bladeAttackDamage = 15.0f;

	LaserAttackRange = 1900.0f;
	SwapAttackRange = 1000.0f;
	BladeAttackRange = 200.0f;
}

void AMilitarySoldierAI::OnEnableAI(bool isEnable)
{
	Super::OnEnableAI(isEnable);
	StateReset();
}

void AMilitarySoldierAI::DieProcess()
{
	StateReset();
	Super::DieProcess();
}

void AMilitarySoldierAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitAIStatByType(EHackedAIType::MS_AI);
	MS_AI_Anim = Cast<UMS_AI_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != MS_AI_Anim);
	HpComponent->OnDie.AddUObject(this, &AMilitarySoldierAI::DieSpawnEmitter);

	MS_AI_Anim->OnGetPlayerPosCheck.AddUObject(this, &AMilitarySoldierAI::GetPlayerPos);
	MS_AI_Anim->OnLaserShockCheck.AddUObject(this, &AMilitarySoldierAI::LaserShockDamaging);
	MS_AI_Anim->OnLaserShockEnd.AddUObject(this, &AMilitarySoldierAI::LaserShockEnd);
	MS_AI_Anim->OnBladeShockCheck.AddUObject(this, &AMilitarySoldierAI::BladeShockDamaging);
	MS_AI_Anim->OnRigidityEnd.AddUObject(this, &AMilitarySoldierAI::RigidityEnd);
	MS_AI_Anim->OnMontageEnded.AddDynamic(this, &AMilitarySoldierAI::OnBladeMontageEnded);
	MS_AI_Anim->OnMSAIBeHitEnd.AddUObject(this, &AMilitarySoldierAI::OnEndHitAnimation);
}

// Called when the game starts or when spawned
void AMilitarySoldierAI::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
	HpComponent->SetMaxHP(GetMaxHp());

	BindRPCFunction(NetBaseCP, AMilitarySoldierAI, LaserShock);
	BindRPCFunction(NetBaseCP, AMilitarySoldierAI, BladeFirstAttack);
	BindRPCFunction(NetBaseCP, AMilitarySoldierAI, BladeSecondAttack);
}

// Called every frame
void AMilitarySoldierAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AMilitarySoldierAI::LaserShock()
{
	if (NetBaseCP->IsMaster() && !MS_AI_Anim->AnimIsRigidity) {
		RPC(NetBaseCP, AMilitarySoldierAI, LaserShock, ENetRPCType::MULTICAST, true);
	}
	if (IsDie()) return;
	bIsLaserShock = true;
	MS_AI_Anim->AnimIsLaserShock = true;
}

void AMilitarySoldierAI::GetPlayerPos()
{
	GetNearestPlayer();
	FirstTargetingPos = NearestPlayerPos;
}

void AMilitarySoldierAI::LaserShockDamaging()
{
	if (IsDie()) return;
	FVector LaserStartPos = GetMesh()->GetSocketLocation("Gun_dummy");
	GetNearestPlayer(); // Get NearestPlayer를 갱신한다. Slave는 BT가 돌지않아 강제로 해줘야한다.
	FVector TargetingPos = FirstTargetingPos - LaserStartPos;
	TargetingPos.Normalize();
	FVector LaserEndPos = LaserStartPos + TargetingPos * LaserAttackRange;
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(LaserStartPos, NearestPlayerPos);

	PC_LaserShock = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_LaserShock, LaserStartPos, TargetRotation, FVector(1.0f,2.0f,2.0f), true);
	FHitResult HitResult(ForceInit);
	FCollisionQueryParams LaserShockQueryParams(NAME_None, true, this);
	LaserShockQueryParams.AddIgnoredActor(this);

	//DrawDebugLine(GetWorld(), LaserStartPos, LaserEndPos, FColor::Green, false, 2.f, false, 4.f);

	bool bResult = GetWorld()->LineTraceSingleByChannel(HitResult, LaserStartPos, LaserEndPos, ECC_GameTraceChannel10, LaserShockQueryParams);
	FVector HitLocation;

	if (bResult)
	{
		if (HitResult.GetActor())
		{
			HitLocation = HitResult.Location;
			AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(HitResult.GetActor());
			//LOG_SCREEN("Hit Result: %s", *(HitResult.GetActor()->GetName()));
			FDamageEvent DamageEvent;
			if(hackedPC)
			hackedPC->TakeDamage(_laserAttackDamage, DamageEvent, GetController(), this);		
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_LaserShockHit, HitLocation, TargetRotation, FVector(1.0f, 1.0f, 1.0f), true);

			ABarrier* barrier = Cast<ABarrier>(HitResult.GetActor());
			if (barrier)
			{
				if (HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial) {
					TutorialBarrierHitCount++;
				}
				else {
					TutorialBarrierHitCount = 0;
				}
				
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_BarrierHit, HitLocation, TargetRotation, FVector(1.0f, 1.0f, 1.0f), true);
			}
		}
		else
		{
			HitLocation = LaserEndPos;
		}

		if (PC_LaserShock)
		{
			int32 NumberOfBeams = 32;
			for (int32 i = 0; i <= NumberOfBeams; i++)
			{
				PC_LaserShock->SetBeamTargetPoint(i, HitLocation, 0);
			}

			//PC_LaserShock->DestroyComponent();
			//PC_LaserShock = nullptr;
		}

	}

}

void AMilitarySoldierAI::BladeShock(UBehaviorTreeComponent* OwnerComp)
{
	if (bIsBladeAttack || MS_AI_Anim->AnimIsRigidity) return;
	savedCp = OwnerComp;
	if (IsDie()) return;
	if (FMath::RandBool())
	{
		BladeFirstAttack();
	}
	else
	{
		BladeSecondAttack();
	}
}

void AMilitarySoldierAI::BladeShockDamaging()
{
	if (IsDie()) return;
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * BladeAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(70.0f),
		Params);

//#if ENABLE_DRAW_DEBUG
//	FVector TraceVec = GetActorForwardVector() * BladeAttackRange;
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
//	float HalfHeight = BladeAttackRange * 0.5f + 50.0f;
//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
//	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
//	float DebugLifeTime = 5.0f;
//
//	DrawDebugCapsule(GetWorld(),
//		Center,
//		HalfHeight,
//		50.0f,
//		CapsuleRot,
//		DrawColor,
//		false,
//		DebugLifeTime);
//
//#endif
	TArray<AActor*> hitActors;
	for (auto Hit : HitResult)
	{
		if (Hit.GetActor() && !hitActors.Contains(Hit.GetActor()))
		{
			AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(Hit.GetActor());
			if (!hackedPC) continue;
			FDamageEvent DamageEvent;
			hackedPC->TakeDamage(_bladeAttackDamage, DamageEvent, GetController(), this);
			hitActors.Add(Hit.GetActor());
			FVector BladeHitLocation = hackedPC->GetActorLocation();
			//BladeHitLocation.Z += 35.0f;
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_BladeShockHit, BladeHitLocation, GetActorRotation(), FVector(1.0f, 1.0f, 1.0f));
		}
	}
}


void AMilitarySoldierAI::BladeNextAttack()
{
	if (currentMontage == 1)
	{
		BladeSecondAttack();
	}
	else
	{
		BladeFirstAttack();
	}
}

bool AMilitarySoldierAI::OnBladeAttack()
{
	return bIsBladeAttack;
}

void AMilitarySoldierAI::OnBladeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) return;
	if (!bIsBladeAttack) return;
	bIsBladeAttack = false;
	// 존재하면 실행 
	if(savedCp) OnBladeShockEnded.ExecuteIfBound(savedCp);

}

void AMilitarySoldierAI::BladeFirstAttack()
{
	RPC(NetBaseCP, AMilitarySoldierAI, BladeFirstAttack, ENetRPCType::MULTICAST, true);
	currentMontage = 1;
	MS_AI_Anim->PlayBladeMontage();
	MS_AI_Anim->JumpToAttackMontageSection(currentMontage);
	bIsBladeAttack = true;
}

void AMilitarySoldierAI::BladeSecondAttack()
{
	RPC(NetBaseCP, AMilitarySoldierAI, BladeSecondAttack, ENetRPCType::MULTICAST, true);
	currentMontage = 2;
	MS_AI_Anim->PlayBladeMontage();
	MS_AI_Anim->JumpToAttackMontageSection(currentMontage);
	bIsBladeAttack = true;
}

void AMilitarySoldierAI::LaserShockEnd()
{
	bIsLaserShock = false;
	bOnEndLaserShock = true;
	MS_AI_Anim->AnimIsLaserShock = false;
}

void AMilitarySoldierAI::StateReset()
{
	MS_AI_Anim->AnimIsLaserShock = false;
	bIsLaserShock = false;
	MS_AI_Anim->AnimIsBladeShock = false;
	bIsBladeAttack = false;
	MS_AI_Anim->StopMontage();
}

void AMilitarySoldierAI::BeHitAnimation()
{
	if (!bCanBeHitAnimation || MS_AI_Anim->AnimIsBeHit || MS_AI_Anim->AnimIsRigidity) return;
	StateReset();
	MS_AI_Anim->AnimIsBeHit = true;
	Super::BeHitAnimation();
}

void AMilitarySoldierAI::OnEndHitAnimation()
{
	MS_AI_Anim->AnimIsBeHit = false;
	Super::OnEndHitAnimation();

}
void AMilitarySoldierAI::RigidityAI()
{
	if (!bCanRigidity || MS_AI_Anim->AnimIsRigidity) return;
	StateReset();
	MS_AI_Anim->AnimIsRigidity = true;
	Super::RigidityAI();
}

void AMilitarySoldierAI::RigidityEnd()
{
	LOG_SCREEN("MilitarySoldier AI RigidityEnd Check");
	MS_AI_Anim->AnimIsRigidity = false;
	Super::RigidityEnd();
}


