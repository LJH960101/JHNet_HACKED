// Fill out your copyright notice in the Description page of Project Settings.

// Classfication : SecurityRobot using (Laser Sword)
// Weapon : Laser Sword
// Range : Short 

#include "SecurityRobot_LS.h"
#include "InGame/AI/HACKED_AI.h"
#include "SR_LS_AnimInstance.h"
#include "../SMR_AIController.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "../../Stat/HpComponent.h"
#include "InGame/Stat/PlayerDamageEvent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASecurityRobot_LS::ASecurityRobot_LS()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(45.0f, 88.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("SkeletalMesh'/Game/Resources/Enemy/AI/SecurityRobot_LS/Saver_riggiedmesh.Saver_riggiedmesh'"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> SR_LS_AnimationBP(TEXT("AnimBlueprint'/Game/Resources/Enemy/AI/SecurityRobot_LS/SecurityRobot_LS_TEST_BP.SecurityRobot_LS_TEST_BP_C'"));
	if (SR_LS_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SR_LS_AnimationBP.Class);

	}
	
	AIControllerClass = ASMR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	IsAttacking = false;
}

// Called when the game starts or when spawned
void ASecurityRobot_LS::BeginPlay()
{
	Super::BeginPlay();
	HpComponent->SetMaxHP(GetMaxHp());
	BindRPCFunction(NetBaseCP, ASecurityRobot_LS, LS_Attack1);
	BindRPCFunction(NetBaseCP, ASecurityRobot_LS, LS_Attack2);
}

void ASecurityRobot_LS::Reconnect()
{
	Super::Reconnect();
	_ResetAI();
}

void ASecurityRobot_LS::Reconnected()
{
	Super::Reconnected();
	_ResetAI();
}

void ASecurityRobot_LS::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASecurityRobot_LS::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitAIStatByType(EHackedAIType::SR_LS);
	LS_Anim = Cast<USR_LS_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != LS_Anim);
	LS_Anim->OnMontageEnded.AddDynamic(this, &ASecurityRobot_LS::OnAttackMontageEnded);
	LS_Anim->OnAttackHitCheck.AddUObject(this, &ASecurityRobot_LS::LS_Damaging);

}


void ASecurityRobot_LS::LS_Attack(UBehaviorTreeComponent* OwnerComp)
{
	savedCp = OwnerComp;
	if (FMath::RandBool()) LS_Attack1();
	else LS_Attack2();
}

void ASecurityRobot_LS::LS_NextAttack()
{
	if (currentAnimation == 1) LS_Attack2();
	else LS_Attack1();
}

bool ASecurityRobot_LS::OnAttack() {
	return IsAttacking;
}

void ASecurityRobot_LS::LS_Attack1()
{
	RPC(NetBaseCP, ASecurityRobot_LS, LS_Attack1, ENetRPCType::MULTICAST, true);
	currentAnimation = 1;
	LS_Anim->PlayAttackMontage();
	LS_Anim->JumpToAttackMontageSection(currentAnimation);
	IsAttacking = true;
}

void ASecurityRobot_LS::LS_Attack2()
{
	RPC(NetBaseCP, ASecurityRobot_LS, LS_Attack2, ENetRPCType::MULTICAST, true);
	currentAnimation = 2;
	LS_Anim->PlayAttackMontage();
	LS_Anim->JumpToAttackMontageSection(currentAnimation);
	IsAttacking = true;
}

void ASecurityRobot_LS::_ResetAI()
{
	LS_Anim->StopMontage();
	currentAnimation = 0;
	IsAttacking = false;
}

void ASecurityRobot_LS::OnEnableAI(bool isEnable)
{
	Super::OnEnableAI(isEnable);
	if (!isEnable) {
		_ResetAI();
	}
}

void ASecurityRobot_LS::OnSpawn()
{
	Super::OnSpawn();
	LS_Anim->SetDeadAnim(false);
	_ResetAI();
}

void ASecurityRobot_LS::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	CHECK(IsAttacking);
	if (!bInterrupted) {
		IsAttacking = false;
		OnAttackEnd.ExecuteIfBound(savedCp);
	}
}

void ASecurityRobot_LS::LS_Damaging()
{
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * GetAttackRange(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(70.0f),
		Params);

//#if ENABLE_DRAW_DEBUG
//
//	FVector TraceVec = GetActorForwardVector() * GetAttackRange();
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
//	float HalfHeight = GetAttackRange() * 0.5f + 50.0f;
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
//
	TArray<AActor*> hitActors;
	for (auto Hit : HitResult)
	{
		if (Hit.GetActor() && !hitActors.Contains(Hit.GetActor()))
		{
			AHACKEDCharacter* pc = Cast<AHACKEDCharacter>(Hit.GetActor());
			if (!pc) continue;
			FDamageEvent DamageEvent;
			pc->TakeDamage(GetAIAttackDamage(), DamageEvent, GetController(), this);
			hitActors.Add(Hit.GetActor());
		}
	}
}
