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
		bool bIsCooperationFirstStart = false; // Çùµ¿Á¸ Å©·¯¼Å ±Ã±Ø±â »ç¿ë Äù½ºÆ® 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationSecondStart = false; // Çùµ¿Á¸ Å©·¯¼Å ±Ã±Ø±â È°¼ºÈ­ÈÄ AI Ã³Ä¡ Äù½ºÆ® 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationThirdStart = false; // ¿¡½ºÆÛ ½ÎÀÌÅ± ½Çµå »ç¿ë 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Quest, Meta = (AllowPrivateAccess = true))
		bool bIsCooperationFourthStart = false; // Å©·¯¼Å ¹ÙÀÌÅ»¸®Æ¼ ½Çµå »ç¿ë 


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
		void CooperationFirstQuest(); // Çùµ¿ Äù½ºÆ® 1¹ø : Å©·¯¼Å ±Ã±Ø±â »ç¿ë 

	UFUNCTION()
		void CooperationFirstQuestClear(); // Å©·¯¼Å ±Ã±Ø±â »ç¿ë Äù½ºÆ® Å¬¸®¾î 

	UFUNCTION()
		void CooperationSecondQuest(); // Çùµ¿ Äù½ºÆ® 2¹ø : ¿¡½ºÆÛ ½ÎÀÌÅ± ½Çµå »ç¿ë 

	UFUNCTION()
		void CooperationSecondQuestClear(); // Çùµ¿Äù½ºÆ® 2¹ø : Å©·¯¼Å ±Ã±Ø±â¸¦ ¹Þ°í ½ºÆùµÈ AI Ã³Ä¡ 

	UFUNCTION()
		void CooperationThirdQuest(); // Çùµ¿ Äù½ºÆ® 3¹ø : ¿¡½ºÆÛ ½ÎÀÌÅ± ½Çµå 

	UFUNCTION()
		void CooperationThirdQuestClear(); // Çùµ¿ Äù½ºÆ® 3¹ø : ¿¡½ºÆÛ ½ÎÀÌÅ± ½Çµå Å¬¸®¾î 

	UFUNCTION()
		void CooperationFinalQuest(); // Çùµ¿ Äù½ºÆ® 4¹ø : ¹ÙÀÌÅ»¸®Æ¼ ½Çµå 

	UFUNCTION()
		void CooperationFinalQuestClear(); // Çùµ¿ Äù½ºÆ® 4¹ø Å¬¸®¾î ÃÖÁ¾ ¸ðµç Äù½ºÆ® Å¬¸®¾î 




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
	USoundWave* TutorialLoopBgm; // Æ©Åä¸®¾ó ºñÁö¿¥ 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* TutorialStartSound; // Æ©Åä¸®¾ó ÀÔÀå »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* QuestClearSound; // Äù½ºÆ® ¿Ï·á »ç¿îµå

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* GoToCooperationSound; // °³ÀÎ Æ©Åä¸®¾ó Å¬¸®¾î »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* AllTutoClearSound; // ¸ðµç Æ©Åä¸®¾ó Å¬¸®¾î »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* ShelterStartSound; // µ¿¸é»óÅÂ ÇØÁ¦~ ¼£¶ó¼£¶ó

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EDQ_Sound; // ¿¡½ºÆÛ ´ë½¬ Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EPQ_Sound; // ¿¡½ºÆÛ ±âº» °ø°Ý Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EM2Q_Sound; // ¿¡½ºÆÛ ½ÎÀÌÅ± µå·Ó Äù½ºÆ® »ç¿îµå

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EFQ_Sound; // ¿¡½ºÆÛ ½ÎÀÌÅ± Æ÷½º Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* ESQ_Sound; // ¿¡½ºÆÛ ½ÎÀÌÅ± ¼îÅ©¿þÀÌºê Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* EUQ_Sound; // ¿¡½ºÆÛ ½ÎÀÌÅ± ¿À¹ö µå¶óÀÌºê Äù½ºÆ® 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CDQ_Sound; // Å©·¯¼Å ´ë½¬ Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CPQ_Sound; // Å©·¯¼Å ±âº» °ø°Ý Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CM2Q_Sound; // Å©·¯¼Å µ¹Áø Äù½ºÆ® »ç¿îµå

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CFQ_Sound; // Å©·¯¼Å º£¸®¾î Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CSQ_Sound; // Å©·¯¼Å ÆÛ´Ï½¬ ½Çµå Äù½ºÆ® »ç¿îµå 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CUQ_Sound; // Å©·¯¼Å ±Ã±Ø±â Äù½ºÆ® 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CZFQ_Sound; // Å©·¯¼Å ¾Æµå·¹³¯¸° ºÐÃâ Äù½ºÆ® 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CZSQ_Sound; // ¿¡½ºÆÛ ½ÎÀÌÅ± ½Çµå Äù½ºÆ® 

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundWave* CZTQ_Sound; // Å©·¯¼Å ¹ÙÀÌÅ»¸®Æ¼ ½Çµå Äù½ºÆ® 




	







	



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
