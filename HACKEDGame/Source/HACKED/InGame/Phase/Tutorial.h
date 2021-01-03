// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/HACKEDInGameManager.h"
#include "GameFramework/Actor.h"
#include "Tutorial.generated.h"

	// Spawn Object types
UENUM(BlueprintType)
enum class ETutorialAIType : uint8
{
	MS_AI		UMETA(DisplayName = "MS_AI"),
	MN_AI		UMETA(DisplayName = "MN_AI"),
	MAX
};

UCLASS()
class HACKED_API ATutorial : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ATutorial();



private:

	FORCEINLINE bool IsEsperPlay();
	FORCEINLINE bool IsCrusherPlay();

	class AHACKED_ESPER* Esper;

	class AHACKED_CRUSHER* Crusher;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
		class UNetworkBaseCP* NetBaseCP;

	UPROPERTY(EditDefaultsOnly, Category = "MilitarySoldierAI")
		TSubclassOf<class AMilitarySoldierAI> MilitarySoldierBP;

	UPROPERTY(EditDefaultsOnly, Category = "MilitaryNamedAI")
		TSubclassOf<class AMilitaryNamedAI> MilitaryNamedBP;

	UPROPERTY(EditDefaultsOnly, Category = "HomingMissile")
		TSubclassOf<class AHomingMissile> HomingMissileBP;

	UPROPERTY(EditAnywhere, Category = TutorialSpawner)
		TArray<AActor*> EsperTutorialSpawner;

	UPROPERTY(EditAnywhere, Category = TutorialSpawner)
		TArray<AActor*> CrusherTutorialSpawner;

	UPROPERTY(EditAnywhere, Category = TutorialSpawner)
		TArray<AActor*> CooperationTutorialSpawner;

	UPROPERTY(EditAnywhere, Category = TutorialSpawner)
		AActor* MissileSpawner;

	UPROPERTY(EditAnywhere, Category = TutorialSkip)
		TArray<AActor*> CooperationZonePos;

	UPROPERTY(EditAnywhere, Category = TutorialSkip)
		TArray<AActor*> ShelterPos;

	UPROPERTY(EditAnywhere)
		AStaticMeshActor* MissileSpawnerMesh;
	UPROPERTY(EditAnywhere)
		AStaticMeshActor* MissileSpawnerBodyMesh;


private:
	
	FTimerHandle EsperSpawnDelayTimer;

	FTimerHandle CrusherSpawnDelayTimer;

	FTimerHandle CooperationZoneDelayTimer;

	FTimerHandle MissileSpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tutorial, Meta = (AllowPrivateAccess = true))
		float SpawnDelayTime = 2.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Tutorial, Meta = (AllowPrivateAccess = true))
		float EsperSpawnDelay;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Tutorial, Meta = (AllowPrivateAccess = true))
		float CrusherSpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tutorial, Meta = (AllowPrivateAccess = true))
		float MissileSpawnDelay = 7.0f;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character_Info, Meta = (AllowPrivateAccess = true))
		FVector EsperLocationBack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character_Info, Meta = (AllowPrivateAccess = true))
		FVector CrusherLocationBack;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Tutorial_AI_Info, Meta = (AllowPrivateAccess = true))
		FVector EsperZoneSpawnPos;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Tutorial_AI_Info, Meta = (AllowPrivateAccess = true))
		FVector CrusherZoneSpawnPos;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tutorial_Skip)
		bool bTutorialSkip = false;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool EsperZoneEnemyCheck;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool CrusherZoneEnemyCheck;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool CooperationZoneEnemyCheck;


	//----------------Esper Tutorial Level ----------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperDashStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperPrimaryStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperM2Start = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperFirstStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperSecondStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperUltimateStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperFinalSpawncheck= false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsEsperSkillDisable = true;

	//----------------Crusher Tutorial Level ----------------//

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherDashStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherPrimaryStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherM2Start = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherFirstStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherSecondStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherUltimateStart = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherFinalSpawnCheck = false;
	

	//----------------Crusher Tutorial Level ----------------//
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationStart = true;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationFirstStart = false; // ������ ũ���� �ñر� ��� ����Ʈ 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationSecondStart = false; // ������ ũ���� �ñر� Ȱ��ȭ�� AI óġ ����Ʈ 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationThirdStart = false; // ������ ����ű �ǵ� ��� 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationFourthStart = false; // ũ���� ����Ż��Ƽ �ǵ� ��� 


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tutorial)
		TArray<class AHACKED_AI*> hackedAI;

private:

	class AHACKEDInGameManager* HACKEDInGameManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EsperTutorialCheckingZone, Meta = (AllowPrivateAccess = true))
	class AEmptyZoneFinder* EsperTutorialZoneCheckAI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CrusherTutorialCheckingZone, Meta = (AllowPrivateAccess = true))
	class AEmptyZoneFinder* CrusherTutorialZoneCheckAI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CrusherTutorialCheckingZone, Meta = (AllowPrivateAccess = true))
	class AEmptyZoneFinder* CooperationZoneCheckAI;

public:
	RPC_FUNCTION(ATutorial, RPCTutorialSkip)
	UFUNCTION(BlueprintCallable)
	void RPCTutorialSkip();

	UFUNCTION()
	void TutorialSkip();

	UFUNCTION(BlueprintCallable)
	void ShelterSequence();

	UFUNCTION(BlueprintCallable)
	void TutorialStart();

	UFUNCTION()
	void TutorialSequence();

	
	RPC_FUNCTION(ATutorial, RPCCollisionDisable)
	UFUNCTION()
		void RPCCollisionDisable();

	UFUNCTION()
		void CollisionDisable();
	
private:

	FTimerHandle ShelterSequenceTimer;

	FTimerHandle TutorialSequenceTimer;

private:

	UFUNCTION()
		void TutorialSpawner(ETutorialAIType aIType, TArray<AActor*> spawnPos, bool onlyOne);

	UFUNCTION()
		void TutoMissileSpawner();


private:

	UFUNCTION()
		void EsperAIZoneCheck();

	UPROPERTY(VisibleInstanceOnly, Category = Tutorial_EnemyCount, Meta = (AllowPrivateAccess = true))
	uint32 EsperZoneEnemyCount = 0;

	UPROPERTY(VisibleInstanceOnly, Category = Tutorial_EnemyCount, Meta = (AllowPrivateAccess = true))
	uint32 CrusherZoneEnemyCount = 0;

	UPROPERTY(VisibleInstanceOnly, Category = Tutorial_EnemyCount, Meta = (AllowPrivateAccess = true))
	uint32 CooperationZoneEnemyCount = 0;

	UFUNCTION()
		void CrusherAIZoneCheck();

	UFUNCTION()
		void CooperationZoneAICheck();

private:
	UFUNCTION()
		void QuestClearSubtileShow(bool isEsper);

	UFUNCTION()
		void EsperDashQuest();

	UFUNCTION()
		void EsperDashQuestClear();

	UFUNCTION()
		void EsperPrimaryQuest();

	UFUNCTION()
		void EsperPrimaryQuestClear();

	UFUNCTION()
		void EsperSkillM2Quest();

	UFUNCTION()
		void EsperSkillM2QuestClear();

	UFUNCTION()
		void EsperFirstSkillQuest();

	UFUNCTION()
		void EsperFirstSkillQuestClear();

	UFUNCTION()
		void EsperSecondSkillQuest();

	UFUNCTION()
		void EsperSecondSkillQuestClear();

	UFUNCTION()
		void EsperUltimateSkillQuest();

	UFUNCTION()
		void EsperUltimateQuestSpawn();

	RPC_FUNCTION(ATutorial, EsperUltimateSkillQuestClear)
	UFUNCTION()
		void EsperUltimateSkillQuestClear();

	UFUNCTION()
		void CrusherDashQuest();

	UFUNCTION()
		void CrusherDashQuestClear();

	UFUNCTION()
		void CrusherPrimaryQuest();

	UFUNCTION()
		void CrusherPrimaryQuestClear();

	UFUNCTION()
		void CrusherM2Quest();

	UFUNCTION()
		void CrusherM2QuestClear();

	UFUNCTION()
		void CrusherFirstQuest();

	UFUNCTION()
		void CrusherFirstQuestClear();

	UFUNCTION()
		void CrusherSecondQuest();

	UFUNCTION()
		void CrusherSecondQuestClear();

	UFUNCTION()
		void CrusherUltimateQuest();

	UFUNCTION()
		void CrusherUltimateQuestSpawn();

	RPC_FUNCTION(ATutorial, CrusherUltimateQuestClear)
	UFUNCTION()
		void CrusherUltimateQuestClear();

	UFUNCTION()
		void CooperationFirstQuest(); // ���� ����Ʈ 1�� : ũ���� �ñر� ��� 

	UFUNCTION()
		void CooperationFirstQuestClear(); // ũ���� �ñر� ��� ����Ʈ Ŭ���� 

	UFUNCTION()
		void CooperationSecondQuest(); // ���� ����Ʈ 2�� : ������ ����ű �ǵ� ��� 

	UFUNCTION()
		void CooperationSecondQuestClear(); // ��������Ʈ 2�� : ũ���� �ñر⸦ �ް� ������ AI óġ 

	UFUNCTION()
		void CooperationThirdQuest(); // ���� ����Ʈ 3�� : ������ ����ű �ǵ� 

	UFUNCTION()
		void CooperationThirdQuestClear(); // ���� ����Ʈ 3�� : ������ ����ű �ǵ� Ŭ���� 

	UFUNCTION()
		void CooperationFinalQuest(); // ���� ����Ʈ 4�� : ����Ż��Ƽ �ǵ� 

	UFUNCTION()
		void CooperationFinalQuestClear(); // ���� ����Ʈ 4�� Ŭ���� ���� ��� ����Ʈ Ŭ���� 




private:
	bool _bEsperStartOnce = true;
	bool _bEsperClearOnce = true;

	bool _bCrusherStartOnce = true;
	bool _bCrusherClearOnce = true;

	bool _bCooperationStartOnce = true;
	bool _bCooperationClearOnce = false;

	bool _bCrusherGaugeCheckOnce = true;
	bool _bCooperationZoneAIOnce = true;

	bool _bTutoFinalCheck = true;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* TutorialSoundComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* IntroSoundComp;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundAttenuation* Tutorial_SoundAttenuation;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* TutorialLoopBgm; // Ʃ�丮�� ������ 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* TutorialStartSound; // Ʃ�丮�� ���� ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* QuestClearSound; // ����Ʈ �Ϸ� ����

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* GoToCooperationSound; // ���� Ʃ�丮�� Ŭ���� ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* AllTutoClearSound; // ��� Ʃ�丮�� Ŭ���� ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* ShelterStartSound; // ������� ����~ ���󼣶�

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EDQ_Sound; // ������ �뽬 ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EPQ_Sound; // ������ �⺻ ���� ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EM2Q_Sound; // ������ ����ű ��� ����Ʈ ����

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EFQ_Sound; // ������ ����ű ���� ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* ESQ_Sound; // ������ ����ű ��ũ���̺� ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EUQ_Sound; // ������ ����ű ���� ����̺� ����Ʈ 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CDQ_Sound; // ũ���� �뽬 ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CPQ_Sound; // ũ���� �⺻ ���� ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CM2Q_Sound; // ũ���� ���� ����Ʈ ����

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CFQ_Sound; // ũ���� ������ ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CSQ_Sound; // ũ���� �۴Ͻ� �ǵ� ����Ʈ ���� 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CUQ_Sound; // ũ���� �ñر� ����Ʈ 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CZFQ_Sound; // ũ���� �Ƶ巹���� ���� ����Ʈ 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CZSQ_Sound; // ������ ����ű �ǵ� ����Ʈ 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CZTQ_Sound; // ũ���� ����Ż��Ƽ �ǵ� ����Ʈ 




	







	



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
