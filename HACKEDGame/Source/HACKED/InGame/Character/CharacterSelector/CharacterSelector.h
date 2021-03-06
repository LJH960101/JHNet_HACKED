// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Pawn.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "InGame/Network/ReplicatableVar.h"
#include "InGame/Interface/Networkable.h"
#include "CharacterSelector.generated.h"

enum class ESelectState : uint8{
	NOT_SELECTED = 0,
	MASTER_SELECTED,
	SLAVE_SELECTED,
	MAX
};

UCLASS()
class HACKED_API ACharacterSelector : public APawn, public INetworkable
{
	GENERATED_BODY()

public:
	// 재접속당한 유저에서 호출됨.(마스터)
	virtual void Reconnected() override;

	// 단절 통보를 받음.
	virtual void Disconnected() override;

	// Sets default values for this pawn's properties
	ACharacterSelector();

	// On Select in Widget
	UFUNCTION(BlueprintCallable, Category = "Character Select")
	void Select(bool isEsper);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Character Select")
	void OnClick(bool bIsEsper, bool bIsOn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Character Select")
	void OnEndCharacterSelect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Character Select")
	void OnLock(bool isOn);

	UFUNCTION(BlueprintCallable, Category = "Character Select")
	bool IsEndSelect();

private:
	// 선택을 처리하는 함수. 마스터에서 재생됨.
	RPC_FUNCTION(ACharacterSelector, _MasterSelect, int, bool);
	void _MasterSelect(int slot, bool isEsper);
	
	// 선택됨을 통보 받는 함수.
	RPC_FUNCTION(ACharacterSelector, _RPCSelect, int, bool, bool);
	void _RPCSelect(int slot, bool isEsper, bool isOn);

	RPC_FUNCTION(ACharacterSelector, _EndSelect);
	void _EndSelect();

	RPC_FUNCTION(ACharacterSelector, _RPCRefreshAuthority)
	void _RPCRefreshAuthority();

	RPC_FUNCTION(ACharacterSelector, _RPCRecvAuthority, FVector, FVector)
	void _RPCRecvAuthority(FVector crusherPos, FVector esperPos);

	void _RemoveWidget();

	// 스킵 설정되어있을경우 스킵하는 함수.
	void _SkipCheck();
	FTimerHandle _skipTimerHandle;

	ESelectState crusherState, esperState;

	CReplicatableVar<bool> _bEndSelect;

	class AHACKEDInGameManager* HACKEDInGameManager;
	class AHACKED_ESPER* _esperActor;
	class AHACKED_CRUSHER* _crusherActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tutorial, Meta = (AllowPrivateAccess = true))
		class ATutorial* Tutorial;
public:


private:
	UFUNCTION()
		void GameIntroStart();

	UFUNCTION()
		void GameIntroFadeInOut();

	UFUNCTION()
		void GameIntroFinish();

	UFUNCTION()
		void OnTutorialStart();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkipIntro, Meta = (AllowPrivateAccess = true))
	bool bEsperSelectSkip = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkipIntro, Meta = (AllowPrivateAccess = true))
	bool bCrusherSelectSkip = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkipIntro, Meta = (AllowPrivateAccess = true))
	bool bTutoSkipSequence = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Select")
	bool bSkip = false;

	// Play to esper when skip? (false is crusher)
	UPROPERTY(BlueprintReadOnly, Category = "Character Select")
	bool bSkipToEsper = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UArrowComponent* _arrowCP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* _cameraCP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network, meta = (AllowPrivateAccess = "true"))
	class UNetworkBaseCP* NetBaseCP;

	UPROPERTY(EditAnywhere)
	class UAudioComponent* TutorialSoundComp;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* DashQuestSound;

private:
	UPROPERTY()
	TSubclassOf<class UWGCharacterSelect> WG_CharacterSelect;
	UPROPERTY()
	class UWGCharacterSelect* _characterSelectWidget = nullptr;

	FTimerHandle GameStartTimerHandle;

};