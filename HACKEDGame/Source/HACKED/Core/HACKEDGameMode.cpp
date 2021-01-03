// Fill out your copyright notice in the Description page of Project Settings.
#include "HACKEDGameMode.h"
#include "InGame/Phase/Phase3.h"
#include "InGame/Phase/Tutorial.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"

void AHACKEDGameMode::AIAllDie()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHACKED_AI::StaticClass(), outActors);

	FDamageEvent DamageEvent;
	for (int i = 0; i < outActors.Num(); ++i) {
		outActors[i]->TakeDamage(10000, DamageEvent, GetWorld()->GetFirstPlayerController(),
			GetWorld()->GetFirstPlayerController()->GetOwner());
	}
}

void AHACKEDGameMode::SkipTutorial()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutorial::StaticClass(), outActors);

	for (auto actor : outActors) {
		ATutorial* Tutorial = Cast<ATutorial>(actor);
		if (!Tutorial) continue;
		Tutorial->bTutorialSkip = true;
		Tutorial->RPCTutorialSkip();
	}
}

void AHACKEDGameMode::GoToBoss()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhase3::StaticClass(), outActors);
	for (auto actor : outActors) {
		APhase3* Phase3 = Cast<APhase3>(actor);
		if(!Phase3) continue;
		Phase3->RPCGoToBossCheat();
	}
}

void AHACKEDGameMode::BossEasyMode()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMilitaryBossAI::StaticClass(), outActors);
	for (auto actor : outActors)
	{
		AMilitaryBossAI* militaryBoss = Cast<AMilitaryBossAI>(actor);
		if(!militaryBoss) continue;
		militaryBoss->RPCSetBossDifficulty(1);
	}
}

void AHACKEDGameMode::BossNormalMode()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMilitaryBossAI::StaticClass(), outActors);
	for (auto actor : outActors)
	{
		AMilitaryBossAI* militaryBoss = Cast<AMilitaryBossAI>(actor);
		if (!militaryBoss) continue;
		militaryBoss->RPCSetBossDifficulty(2);
	}
}

void AHACKEDGameMode::BossHardMode()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMilitaryBossAI::StaticClass(), outActors);
	for (auto actor : outActors)
	{
		AMilitaryBossAI* militaryBoss = Cast<AMilitaryBossAI>(actor);
		if (!militaryBoss) continue;
		militaryBoss->RPCSetBossDifficulty(3);
	}
}
