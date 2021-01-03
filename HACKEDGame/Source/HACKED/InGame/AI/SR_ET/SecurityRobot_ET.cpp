// Fill out your copyright notice in the Description page of Project Settings.

// Classfication : SecurityRobot using (Electric Shock)
// Weapon : Electric Shock
// Range : Medium 

#include "SecurityRobot_ET.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "SR_ET_AnimInstance.h"
#include "../../Stat/HpComponent.h"
#include "../SMR_AIController.h"

// Sets default values
ASecurityRobot_ET::ASecurityRobot_ET()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(50.0f, 88.0f);

	//GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("SkeletalMesh'/Game/Resources/Enemy/AI/SecurityRobot_ET/Ball_riggiedmesh.Ball_riggiedmesh'"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> SR_ET_AnimationBP(TEXT("AnimBlueprint'/Game/Resources/Enemy/AI/SecurityRobot_ET/SecurityRobot_ET_TEST_BP.SecurityRobot_ET_TEST_BP_C'"));
	if (SR_ET_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SR_ET_AnimationBP.Class);
	}

	AIControllerClass = ASMR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ASecurityRobot_ET::BeginPlay()
{
	Super::BeginPlay();
	HpComponent->SetMaxHP(GetMaxHp());

	BindRPCFunction(NetBaseCP, ASecurityRobot_ET, ElectricShock);
}

void ASecurityRobot_ET::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASecurityRobot_ET::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitAIStatByType(EHackedAIType::SR_ET);
	SR_ET_Anim = Cast<USR_ET_AnimInstance>(GetMesh()->GetAnimInstance());
	SR_ET_Anim->OnElectricShockCheck.AddUObject(this, &ASecurityRobot_ET::ElectricShockDamaging);
	SR_ET_Anim->OnElectricShockCheck.AddLambda([this]()->void {
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		});
	SR_ET_Anim->OnElectricShockEnd.AddUObject(this, &ASecurityRobot_ET::OnAttackEnd);

	SR_ET_Anim->OnElectricShockFinish.AddLambda([this]()->void {
		GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
	});
}

// Called to bind functionality to input
void ASecurityRobot_ET::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASecurityRobot_ET::ET_Rigidity()
{
	SR_ET_Anim->AnimIsRigidity = true;
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
}

void ASecurityRobot_ET::ElectricShock()
{
	RPC(NetBaseCP, ASecurityRobot_ET, ElectricShock, ENetRPCType::MULTICAST, true);
	IsElectricShock = true;
	SR_ET_Anim->AnimIsElectricShock = true;
}

void ASecurityRobot_ET::ElectricShockDamaging()
{
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * GetAttackRange(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(50.0f),
		Params);

	//LOG_SCREEN("ET Attack Range : %f", GetAttackRange());

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

	for (auto Hit : HitResult)
	{
		if (Hit.GetActor())
		{
			AHACKEDCharacter* pc = Cast<AHACKEDCharacter>(Hit.GetActor());
			if (!pc) continue;
			FDamageEvent DamageEvent;
			pc->TakeDamage(GetAIAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

void ASecurityRobot_ET::OnAttackEnd()
{
	bOnEndAttack = true;
}

void ASecurityRobot_ET::DieProcess()
{
	Super::DieProcess();
	_ResetAI();
}


void ASecurityRobot_ET::OnSpawn()
{
	Super::OnSpawn();
	SR_ET_Anim->SetDeadAnim(false);
	_ResetAI();
}

void ASecurityRobot_ET::_ResetAI()
{
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
	IsElectricShock = false;
	SR_ET_Anim->AnimIsElectricShock = false;
	bOnEndAttack = false;
} 