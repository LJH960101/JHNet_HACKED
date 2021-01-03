// Fill out your copyright notice in the Description page of Project Settings.

#include "JHNETGameMode.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "InGame/InGameManager.h"
#include "InGame/Network/IngameNetworkProcessor.h"

void AJHNETGameMode::DisconnectTest()
{
	auto gameInstance = Cast<UJHNETGameInstance>(GetGameInstance());
	if (gameInstance) {
		auto networkSystem = Cast<UNetworkSystem>(gameInstance->GetNetworkSystem());
		if (networkSystem) {
			networkSystem->Shutdown();
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("DisconnectTest"));
		}
	}
}

void AJHNETGameMode::RecvErrorTest()
{
	auto gameInstance = Cast<UJHNETGameInstance>(GetGameInstance());
	if (gameInstance) {
		auto networkSystem = Cast<UNetworkSystem>(gameInstance->GetNetworkSystem());
		if (networkSystem) networkSystem->SetOnDebugReset();
	}
}

void AJHNETGameMode::IsServer()
{
	auto gameInstance = Cast<UJHNETGameInstance>(GetGameInstance());
	if (gameInstance) {
		auto networkSystem = Cast<UNetworkSystem>(gameInstance->GetNetworkSystem());
		if (networkSystem) JHNET_LOG_SCREEN("Slot : %d", networkSystem->GetCurrentSlot());
	}
}

void AJHNETGameMode::ForceReconnect()
{
	auto gameInstance = Cast<UJHNETGameInstance>(GetGameInstance());
	if (gameInstance) {
		auto networkSystem = Cast<UNetworkSystem>(gameInstance->GetNetworkSystem());
		if (networkSystem) {
			UINT64 slots[MAX_PLAYER];
			for (int i = 0; i < MAX_PLAYER; ++i) {
				slots[i] = networkSystem->GetSteamID(i);
			}
			networkSystem->Shutdown();
			for (int i = 0; i < MAX_PLAYER; ++i) {
				networkSystem->SetSteamID(i, slots[i]);
			}
			Sleep(1000);
			networkSystem->Init();
		}
	}
}

void AJHNETGameMode::ForeceEndGame()
{
	auto gameInstance = Cast<UJHNETGameInstance>(GetGameInstance());
	if (gameInstance) {
		auto networkSystem = Cast<UNetworkSystem>(gameInstance->GetNetworkSystem());
		if (networkSystem) {
			networkSystem->GetInGameManager()->GetNetworkProcessor()->EndGame();
		}
	}
}
