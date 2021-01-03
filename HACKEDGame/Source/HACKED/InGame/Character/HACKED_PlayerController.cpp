// Fill out your copyright notice in the Description page of Project Settings.

#include "HACKED_PlayerController.h"
#include "InGame/Character/HACKEDCharacter.h"

AHACKED_PlayerController::AHACKED_PlayerController() {
	bReplicates = false;
}

void AHACKED_PlayerController::OnPossess(APawn* aPawn) {
	Super::OnPossess(aPawn);
	AHACKEDCharacter* hackedCharacter = Cast<AHACKEDCharacter>(aPawn);
	if (hackedCharacter) {
		hackedCharacter->Possessed(this);
	}
}