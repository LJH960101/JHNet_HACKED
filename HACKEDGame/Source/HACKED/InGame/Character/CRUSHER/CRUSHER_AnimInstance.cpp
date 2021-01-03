// Fill out your copyright notice in the Description page of Project Settings.


#include "CRUSHER_AnimInstance.h"
#include "../../AI/HACKED_AI.h"
#include "HACKED_CRUSHER.h"
#include "DrawDebugHelpers.h"
#include <Runtime\Engine\Private\KismetTraceUtils.h>
#include "Barrier.h"
#include "Ingame/Phase/ExplosiveObject.h"

UCRUSHER_AnimInstance::UCRUSHER_AnimInstance()
{
	//Montage Initialize
	static ConstructorHelpers::FObjectFinder<UAnimMontage>ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Resources/Character/Crusher/FBX/Animations/Attack_Montage.Attack_Montage'"));
	if (ATTACK_MONTAGE.Succeeded()){attackMontage = ATTACK_MONTAGE.Object;}
	static ConstructorHelpers::FObjectFinder<UAnimMontage>ULTIMATE_ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Resources/Character/Crusher/FBX/Animations/Ultimate_Attack_Montage.Ultimate_Attack_Montage'"));
	if (ULTIMATE_ATTACK_MONTAGE.Succeeded()){ultimateAttackMontage = ULTIMATE_ATTACK_MONTAGE.Object;}
	static ConstructorHelpers::FObjectFinder<UAnimMontage>PUNISHSHIELD_MONTAGE(TEXT("AnimMontage'/Game/Resources/Character/Crusher/FBX/Animations/PunishShield_Montage.PunishShield_Montage'"));
	if (PUNISHSHIELD_MONTAGE.Succeeded())
	{
		PunishMontage = PUNISHSHIELD_MONTAGE.Object;
	}
	//Sound Initialize
	static ConstructorHelpers::FObjectFinder<USoundBase>SB_ENEMYHIT1(TEXT("SoundWave'/Game/Sound/Crusher/Crasher_Normal_Hit_01.Crasher_Normal_Hit_01'"));
	if (SB_ENEMYHIT1.Succeeded()){enemyAttackSound1 = SB_ENEMYHIT1.Object;}
	static ConstructorHelpers::FObjectFinder<USoundBase>SB_ENEMYHIT2(TEXT("SoundWave'/Game/Sound/Crusher/Crasher_Normal_Hit_02.Crasher_Normal_Hit_02'"));
	if (SB_ENEMYHIT2.Succeeded()){enemyAttackSound2 = SB_ENEMYHIT2.Object;}
	static ConstructorHelpers::FObjectFinder<USoundBase>SB_ENEMYHIT3(TEXT("SoundWave'/Game/Sound/Crusher/Crasher_Normal_Hit_03.Crasher_Normal_Hit_03'"));
	if (SB_ENEMYHIT3.Succeeded()){enemyAttackSound3 = SB_ENEMYHIT3.Object;}
	static ConstructorHelpers::FObjectFinder<USoundAttenuation>SA_CRUSHER_ATTACK(TEXT("SoundAttenuation'/Game/Sound/Attenuations/CRUSHER/CRUSHER_ATTACK.CRUSHER_ATTACK'"));
	if (SA_CRUSHER_ATTACK.Succeeded()){crusher_attack_attenuation = SA_CRUSHER_ATTACK.Object;}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ATTACKDECAL(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Material_Inst_MI/Use_Crusher_Skill02/MM_Decal_Inst2.MM_Decal_Inst2"));
	if (ATTACKDECAL.Succeeded())
	{
		DC_CrusherAttack = ATTACKDECAL.Object;
	}

}

void UCRUSHER_AnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	player = Cast<AHACKED_CRUSHER>(TryGetPawnOwner());
	if (player)
	{
		HLOG("Player Found ! : %s", *player->GetName());
		checkSphere = player->GetCheckSphere();

	}
	checkSphere->Deactivate();
}

void UCRUSHER_AnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	//Sync Player Data
	if (player)
	{
		ValidatePlayerData();
	}

	//HLOG("CURRENT SECTION NUMBER : %d", currentAttackSectionNumber);
	//HLOG("CURRENT COMBO : %d", currentComboNumber);

	//Keep Validate bIsAttackState
	if (!IsAttackMontagePlaying())
	{
		bIsAttackState = false;
		currentComboNumber = 0;
		currentAttackSectionNumber = 0;
	}
	else
	{
		bIsAttackState = true;
	}

	//Attack LineTrace Check
	if (isTracerActive)
	{
		UpdateTracerLocation();
		LineTracing();
	}
	else
	{
		bIsHit = false;
		bDoOnce = false;
	}
}

void UCRUSHER_AnimInstance::ValidatePlayerData()
{
	SetRocketRushingState(player->GetRocketRushLaunchingState());
	SetRocketChargingState(player->GetRocketRushChargingState());
	bIsAdrenalSurge = player->bIsAdrenalSurge;
}

void UCRUSHER_AnimInstance::PlayAttackMontage()
{
	//If Montage isn't playing, Play it
	if (!IsAttackMontagePlaying())
	{
		if (bIsAdrenalSurge)
		{
			Montage_Play(ultimateAttackMontage);
		}
		else
		{
			Montage_Play(attackMontage);
		}
		currentAttackSectionNumber = 0;
		currentComboNumber = 0;
		bCanDoNextAttackInput = true;
	}
}

void UCRUSHER_AnimInstance::DoNextAttack()
{
	if (bCanDoNextAttackInput)
	{
		if(bIsAdrenalSurge)
		{
			switch (currentAttackSectionNumber)
			{
			case 0:
				currentAttackSectionNumber = 1;
				currentComboNumber = 1;
				break;
			case 1:
				Montage_SetNextSection(FName("Attack0"), FName("Attack1"), ultimateAttackMontage);
				currentAttackSectionNumber = 2;
				currentComboNumber = 2;
				break;
			case 2:
				Montage_SetNextSection(FName("Attack1"), FName("Attack2"), ultimateAttackMontage);
				currentAttackSectionNumber = 3;
				currentComboNumber = 3;
				break;
			default:
				//If the value exceed 0~2 change it to 0
				currentAttackSectionNumber = 0;
				currentComboNumber = 0;
				break;
			}
		}
		else
		{
			switch (currentAttackSectionNumber)
			{
			case 0:
				currentAttackSectionNumber=1;
				currentComboNumber = 1;
				break;
			case 1:
				Montage_SetNextSection(FName("Attack0"), FName("Attack1"), attackMontage);
				currentAttackSectionNumber=2;
				currentComboNumber = 2;
				break;
			case 2:
				Montage_SetNextSection(FName("Attack1"), FName("Attack2"), attackMontage);
				currentAttackSectionNumber=3;
				currentComboNumber = 3;
				break;
			default:
				//If the value exceed 0~2 change it to 0
				currentAttackSectionNumber = 0;
				currentComboNumber = 0;
				break;
			}
		}
		bCanDoNextAttackInput = false;
	}
}

void UCRUSHER_AnimInstance::UpdateTracerLocation()
{
	attackTraceStartLoc = player->GetMesh()->GetSocketLocation(FName("ShieldTraceStart"));
	attackTraceEndLoc = player->GetMesh()->GetSocketLocation(FName("ShieldTraceEnd"));
}

void UCRUSHER_AnimInstance::LineTracing()
{
	TArray<FHitResult> hit;
	bool ishit = false;
	
	//DrawDebugLine(GetWorld(), attackTraceStartLoc, attackTraceEndLoc, FColor::Green, false, 2.f, false, 4.f);
	//Do LineTrace And Get HACKED_AI to hitted Array while giving them a damage
	GetWorld()->LineTraceMultiByChannel(
		hit,
		attackTraceStartLoc,
		attackTraceEndLoc,
		ECollisionChannel::ECC_GameTraceChannel1);

	if (hit.IsValidIndex(0))
	{
		for (FHitResult hitIter : hit)
		{
			if (hitIter.GetActor()->IsA(AHACKED_AI::StaticClass()))
			{
				AHACKED_AI* hittedEnemy = Cast<AHACKED_AI>(hitIter.GetActor());
				if (!hitted.Contains(hittedEnemy))
				{
					if (!bDoOnce)
					{
						bIsHit = true;
						bDoOnce = true;
						PlayAttackHitSound();
						if (bIsAdrenalSurge)
						{
							if (player->HasAuthority()) {
								GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(player->CS_CrusherUltimateAttack, 1.0f);
								player->GetFollowCamera()->PostProcessSettings.SceneFringeIntensity = 4.0f;
							}
						}
						else
						{
							if (player->HasAuthority()) {
								GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(player->CS_CrusherPA, 1.0f);
							}
						}
					}
					player->SpawnPrimaryHitEffect(hitIter.ImpactPoint);
					//HLOG("%s", *hittedEnemy->GetName());
					GiveDamage(hittedEnemy);
					hitted.Add(hittedEnemy);
				}
			}
			else if (hitIter.GetActor()->IsA(AExplosiveObject::StaticClass()))
			{
				AExplosiveObject* explosive = Cast<AExplosiveObject>(hitIter.GetActor());
				explosive->Explode(player);
			}
		}
		
	}
}

void UCRUSHER_AnimInstance::GiveDamage(AHACKED_AI* damageTo)
{
	//Normal Attack (Not AdrenalSurged)
	if (!bIsAdrenalSurge) {
		FPlayerDamageEvent damageEvent;
		damageEvent.bCanChargeUlt = true;
		damageEvent.bCanHitAnimation = true;

		switch (currentComboNumber)
		{
		case 1:
			damageTo->TakeDamage(player->normalDamage, damageEvent, player->GetController(), player);
			break;
		case 2:
			damageTo->TakeDamage(player->normalDamage, damageEvent, player->GetController(), player);
			break;
		case 3:
			damageTo->TakeDamage(player->criticalDamage, damageEvent, player->GetController(), player);
			break;
		default:
			break;
		}
	}
	//Ultimate Attack (AdrenalSurged)
	else
	{
		FPlayerDamageEvent damageEvent;
		damageEvent.bCanHitAnimation = true;

		switch (currentComboNumber)
		{
		case 1:
			damageTo->TakeDamage(player->ultNormalDamage, damageEvent, player->GetController(), player);
			break;
		case 2:
			damageTo->TakeDamage(player->ultNormalDamage, damageEvent, player->GetController(), player);
			break;
		default:
			break;
		}
	}
	damageTo->LaunchCharacter((damageTo->GetActorLocation() - player->GetActorLocation()).GetSafeNormal() * 300.0f, true, true);
}

void UCRUSHER_AnimInstance::PlayAttackHitSound()
{
	switch (rand() % 3)
	{
	case 0:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), enemyAttackSound1, player->GetActorLocation(), 1.0f, 1.0f, 0.0f, crusher_attack_attenuation);
		break;
	case 1:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), enemyAttackSound2, player->GetActorLocation(), 1.0f, 1.0f, 0.0f, crusher_attack_attenuation);
		break;
	case 2:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), enemyAttackSound3, player->GetActorLocation(), 1.0f, 1.0f, 0.0f, crusher_attack_attenuation);
		break;
	default:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), enemyAttackSound1, player->GetActorLocation(), 1.0f, 1.0f, 0.0f, crusher_attack_attenuation);
		break;
	}

}

void UCRUSHER_AnimInstance::AnimNotify_CheckNextAttack()
{
	if (!bCanDoNextAttackInput)
		bCanDoNextAttackInput = true;
	//HLOG("CHECK NEXT ATTACK");
}

void UCRUSHER_AnimInstance::AnimNotify_StartShieldAttackHitTrace()
{
	isTracerActive = true;

	//HLOG("START SHIELD TRACE");
}

void UCRUSHER_AnimInstance::AnimNotify_StopShieldAttackHitTrace()
{
	isTracerActive = false;
	hitted.Empty();
	bDoOnce = false;
	//HLOG("STOP SHIELD TRACE");
}

void UCRUSHER_AnimInstance::AnimNotify_SpawnBarrier()
{
	FVector BarrierSpawnPos = player->GetActorLocation();
	BarrierSpawnPos.Z -= 30.0f;
	GetWorld()->SpawnActor<ABarrier>(BarrierSpawnPos, FRotator::ZeroRotator);
}

void UCRUSHER_AnimInstance::AnimNotify_BarrierAnimationEnd()
{
	SetBarrierGeneratorActive(false);
	if (!player->bIsSelfHealing)
	{
		player->SetEnableInput();
	}
}

void UCRUSHER_AnimInstance::PlayPunishMontage()
{
	Montage_Play(PunishMontage);
}

void UCRUSHER_AnimInstance::AnimNotify_PunishShieldDamaging()
{
	PunishShieldDamageCheck.Broadcast();

	FVector DecalLocation = player->GetMesh()->GetSocketLocation(FName("ShieldTraceEnd"));
	//DecalLocation.X += 300.0f;
	UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DC_CrusherAttack, FVector(200.0f, 200.0f, 200.0f), DecalLocation, FRotator(-90.0f, 0.0f, 0.0f), 1.0f);
	
	player->SpawnUltimateDustEffectComp(player->GetActorLocation());

	if (player->HasAuthority()) {
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(player->CS_GroundSmash, 1.0f);
	}

}

void UCRUSHER_AnimInstance::AnimNotify_PunishShieldEnd()
{
	AnimIsPunishShield = false;
	PunishShieldEndCheck.Broadcast();
}

void UCRUSHER_AnimInstance::AnimNotify_UltimateOn()
{
	AdrenalSurgeCheck.Broadcast();
	AnimIsAdrenalSurge = false;
}

void UCRUSHER_AnimInstance::AnimNotify_GroundSmash()
{	
	checkSphere->Activate();

	if (bIsAdrenalSurge)
	{
		checkSphere->SetSphereRadius(player->ultGroundSmashRadius);
		// DrawDebugSphere(GetWorld(), player->GetActorLocation(), ultGroundSmashRadius, 0, FColor::Yellow, false, 2.0f);
	}
	else
	{
		checkSphere->SetSphereRadius(player->groundSmashRadius);
		// DrawDebugSphere(GetWorld(), player->GetActorLocation(), groundSmashRadius, 0, FColor::Magenta, false, 2.0f);
	}
	checkSphere->SetWorldLocation(player->GetActorLocation());
	TArray<AActor*> enemies;
	checkSphere->GetOverlappingActors(enemies, TSubclassOf<AHACKED_AI>());
	for (AActor* iter : enemies)
	{
		HLOG("GROUND SMASHED : %s", *iter->GetName());
		if (!iter->IsA(AHACKED_AI::StaticClass()))
			continue;
		AHACKED_AI* enemy = Cast<AHACKED_AI>(iter);
		if (bIsAdrenalSurge)
		{
			FPlayerDamageEvent damageEvent;
			damageEvent.bCanHitAnimation = true;
			damageEvent.bCanRigidity = true;
			iter->TakeDamage(player->ultGroundSmashDamage, damageEvent, player->GetController(), player);
		}
		else
		{
			FPlayerDamageEvent damageEvent;
			damageEvent.bCanChargeUlt = true;
			damageEvent.bCanHitAnimation = true;
			damageEvent.bCanRigidity = true;
			iter->TakeDamage(player->ultGroundSmashDamage, damageEvent, player->GetController(), player);
		}
		enemy->LaunchCharacter((enemy->GetActorLocation() - player->GetActorLocation()).GetSafeNormal() * 400.0f, true, true);
	}

	FVector DecalLocation = player->GetMesh()->GetSocketLocation(FName("ShieldTraceEnd"));
	//DecalLocation.X += 300.0f;
	UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DC_CrusherAttack, FVector(200.0f, 200.0f, 200.0f), DecalLocation, FRotator(-90.0f, 0.0f, 0.0f), 1.0f);

	PlayAttackHitSound();
	player->SpawnUltimateDustEffectComp(player->GetActorLocation());

	if (player->HasAuthority()) {
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(player->CS_GroundSmash, 1.0f);
	}

	checkSphere->Deactivate();
}

void UCRUSHER_AnimInstance::AnimNotify_VitalityShieldCheck()
{
	VitalityShieldCheck.Broadcast();
}

void UCRUSHER_AnimInstance::AnimNotify_VitalityShieldAnimEnd()
{
	VitalityShieldAnimEnd.Broadcast();
	AnimIsVitalityShield = false;
}

void UCRUSHER_AnimInstance::AnimNotify_CrusherDie()
{
	CrusherDieEnd.Broadcast();
}

void UCRUSHER_AnimInstance::AnimNotify_CrusherSelfHealingEnd()
{
	CrusherSelfHealingEnd.Broadcast();
}

void UCRUSHER_AnimInstance::AnimNotify_DashEnd()
{
	CrusherDashEnd.Broadcast();
	AnimIsCrusherDash = false;
}
