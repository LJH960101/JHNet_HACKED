// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "SecurityRobot_SLS.h"
#include "InGame/AI/HACKED_AI.h"
#include "SR_SLS_AnimInstance.h"
#include "../SMR_AIController.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "../../Stat/HpComponent.h"
#include "InGame/Stat/PlayerDamageEvent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASecurityRobot_SLS::ASecurityRobot_SLS()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(50.0f, 88.0f);

	//GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(0.0f, -90.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SR_SLS_SK(TEXT("SkeletalMesh'/Game/Resources/Enemy/AI/SecurityRobot_SLS/SecurityRobot_SLS_Test_SK.SecurityRobot_SLS_Test_SK'"));
	if (SR_SLS_SK.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SR_SLS_SK.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> SR_SLS_AnimationBP(TEXT("AnimBlueprint'/Game/Resources/Enemy/AI/SecurityRobot_SLS/SecurityRobot_SLS_Test_AnimBP.SecurityRobot_SLS_Test_AnimBP_C'"));
	if (SR_SLS_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SR_SLS_AnimationBP.Class);
	}


	AIControllerClass = ASMR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bIsAttacking = false;
	MaxCombo = 3;
	SLSAttackEndComboState();
}

// Called when the game starts or when spawned
void ASecurityRobot_SLS::BeginPlay()
{
	Super::BeginPlay();
	HpComponent->SetMaxHP(GetMaxHp());
	//LOG_SCREEN("SLS SetMaxHp : %f", HpComponent->GetHP());
}


float ASecurityRobot_SLS::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float hp = HpComponent->TakeDamage(DamageAmount);
	return hp;
}

// Called every frame
void ASecurityRobot_SLS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//LOG_SCREEN("SLS SetMaxHp : %f", HpComponent->GetHP());
}

void ASecurityRobot_SLS::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SLS_Anim = Cast<USR_SLS_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != SLS_Anim);
	InitAIStatByType(EHackedAIType::SR_SLS);
	SLS_Anim->OnMontageEnded.AddDynamic(this, &ASecurityRobot_SLS::OnSLSAttackMontageEnded);
	SLS_Anim->OnSLSAttackHitCheck.AddUObject(this, &ASecurityRobot_SLS::SLSAttackDamaging);
	SLS_Anim->OnSLSNextAttackCheck.AddLambda([this]()->void{
		LOG(Warning, "OnSLSNextAttackCheck");
		bCanNextCombo = false;
		if (bIsComboInputOn)
		{
			SLSAttackStartComboState();
			SLS_Anim->JumpToSLSAttackMontageSection(CurrentCombo);
		}
		
	});
}

void ASecurityRobot_SLS::OnSpawn()
{
	Super::OnSpawn();
	SLSAttackEndComboState();
	bIsAttacking = false;
	SLS_Anim->StopAllMontages(0.1f);
}

void ASecurityRobot_SLS::SLSAttack()
{
	if (bIsAttacking)
	{
		CHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (bCanNextCombo)
		{
			bIsComboInputOn = true;
		}
	}
	else
	{
		CHECK(CurrentCombo == 0);
		SLSAttackStartComboState();
		SLS_Anim->PlaySLSAttackMontage();
		SLS_Anim->JumpToSLSAttackMontageSection(CurrentCombo);
		bIsAttacking = true;
	}
}

void ASecurityRobot_SLS::OnSLSAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	CHECK(bIsAttacking);
	CHECK(CurrentCombo > 0);
	bIsAttacking = false;
	SLSAttackEndComboState();
	OnSLSAttackEnd.Broadcast();

}

void ASecurityRobot_SLS::SLSAttackStartComboState()
{
	bCanNextCombo = true;
	bIsComboInputOn = false;
	CHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	
}

void ASecurityRobot_SLS::SLSAttackEndComboState()
{
	bIsComboInputOn = false;
	bCanNextCombo = false;
	CurrentCombo = 0;
}

void ASecurityRobot_SLS::SLSAttackDamaging()
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

