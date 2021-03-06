// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "CharacterSelector.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "InGame/UI/WGCharacterSelect.h"
#include "Core/Network/NetworkSystem.h"
#include "InGame/Character/ESPER/HACKED_ESPER.h"
#include "InGame/Character/CRUSHER/HACKED_CRUSHER.h"
#include "NetworkModule/ConfigParser/ConfigParser.h"
#include "InGame/HACKEDTool.h"
#include "InGame/HACKEDInGameManager.h"

// Sets default values
ACharacterSelector::ACharacterSelector()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UWGCharacterSelect> csWG(TEXT("/Game/Blueprint/UI/InGame/Common/CharacterSelect/WG_CharacterSelect.WG_CharacterSelect_C"));
	if (csWG.Succeeded()) {
		WG_CharacterSelect = csWG.Class;
	}

	TutorialSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	_arrowCP = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	_cameraCP = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));

	RootComponent = _arrowCP;
	_cameraCP->SetupAttachment(_arrowCP);

	crusherState = ESelectState::NOT_SELECTED;
	esperState = ESelectState::NOT_SELECTED;
	
	// Disable UE4 replication
	bReplicates = false;
	SetReplicateMovement(false);

	_bEndSelect = false;
}

void ACharacterSelector::Reconnected()
{
	_RPCRefreshAuthority();
}

void ACharacterSelector::Disconnected()
{
	if (!_bEndSelect) return;

	// 자신의 캐릭터를 기준으로 변수를 다시 맞춘다.
	AHACKED_CRUSHER* crusher = Cast<AHACKED_CRUSHER>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (crusher) {
		crusherState = ESelectState::MASTER_SELECTED;
		esperState = ESelectState::SLAVE_SELECTED;
	}
	else {
		crusherState = ESelectState::SLAVE_SELECTED;
		esperState = ESelectState::MASTER_SELECTED;
	}
}

void ACharacterSelector::_RPCRefreshAuthority()
{
	RPC(NetBaseCP, ACharacterSelector, _RPCRefreshAuthority, ENetRPCType::MASTER, true);

	// 마스터가 선택이 완료되지 않았다면 동기화하지않고 진행한당.
	if (!_bEndSelect) return;

	// 정보(선택정보, 위치)를 넘겨주고
	// 스킵을 통보한다.

	// 슬레이브가 에스퍼인지 판단해서 다시 어쏘리티를 부여한다.
	if (crusherState == ESelectState::MASTER_SELECTED) {
		_crusherActor->NetBaseCP->SetAuthority(0);
		_esperActor->NetBaseCP->SetAuthority(1);
	}
	else {
		_crusherActor->NetBaseCP->SetAuthority(1);
		_esperActor->NetBaseCP->SetAuthority(0);
	}

	FVector crusherPos = FVector::ZeroVector;
	FVector esperPos = FVector::ZeroVector;

	bool isServer = GetWorld()->IsServer();
	bool a = isServer;
	bool d = isServer;
	bool c = isServer;

	if (_crusherActor) crusherPos = _crusherActor->GetActorLocation();
	if (_esperActor) esperPos = _esperActor->GetActorLocation();
	_RPCRecvAuthority(crusherPos, esperPos);
}

void ACharacterSelector::_RPCRecvAuthority(FVector crusherPos, FVector esperPos)
{
	RPC(NetBaseCP, ACharacterSelector, _RPCRecvAuthority, ENetRPCType::MULTICAST, true, crusherPos, esperPos);

	// 이미 선택이 끝난 (이미 동기화가 끝난) 사람은 무시한다.
	if (_bEndSelect) return;

	// 동기화 대상은 플레이어블 캐릭터의 위치를 수정한다. (추후 어쏘리티가 나로 바뀌게 되므로 갱신해야함.)
	if (_crusherActor) {
		_crusherActor->SetActorLocation(crusherPos);
	}
	if (_esperActor)
	{
		_esperActor->SetActorLocation(esperPos);
	}
	_RemoveWidget();
}

void ACharacterSelector::_RemoveWidget()
{
	auto controller = GetWorld()->GetFirstPlayerController();
	if (controller) controller->bShowMouseCursor = false;
	if(_characterSelectWidget && _characterSelectWidget->IsInViewport()) _characterSelectWidget->RemoveFromParent();
	_bEndSelect.ForceChange(true);
}

// Called when the game starts or when spawned
void ACharacterSelector::BeginPlay()	
{
	Super::BeginPlay();

	BindHACKEDInGameManager(HACKEDInGameManager	);

	CConfigParser newParser("networkConfig.ini");
	if (newParser.IsSuccess()) {
		if (newParser.Contain("SinglePlay_PlayToEsper")) {
			bSkipToEsper = newParser.GetBool("SinglePlay_PlayToEsper");
		}
	}

	if (!NetBaseCP->GetNetworkSystem()) {
		LOG_SCREEN("HACKEDInGameManager, InGameNetworkProcessor must be placed in level!!!!");
		return;
	}
	_characterSelectWidget = CreateWidget<UWGCharacterSelect>(GetWorld(), WG_CharacterSelect);
	CHECK(_characterSelectWidget);
	_characterSelectWidget->AddToViewport();
	auto controller = GetWorld()->GetFirstPlayerController();
	if(controller) controller->bShowMouseCursor = true;
	BindRPCFunction(NetBaseCP, ACharacterSelector, _MasterSelect);
	BindRPCFunction(NetBaseCP, ACharacterSelector, _RPCSelect);
	BindRPCFunction(NetBaseCP, ACharacterSelector, _EndSelect);
	BindRPCFunction(NetBaseCP, ACharacterSelector, _RPCRefreshAuthority);
	BindRPCFunction(NetBaseCP, ACharacterSelector, _RPCRecvAuthority);

	if (NetBaseCP->GetNetworkSystem()->bNeedToResync)
		bSkip = false;
	else
		GetWorldTimerManager().SetTimer(_skipTimerHandle, this, &ACharacterSelector::_SkipCheck, 0.5f, false);
	HACKEDInGameManager->GetFadeInOutMaker()->FadeIn(1.0f);
	_bEndSelect.Init(&NetBaseCP, TEXT("bEndSelect"));
	_bEndSelect.SetReliable(true);

	// 크러셔와 에스퍼 액터를 받는다.
	_crusherActor = HACKEDInGameManager->GetCrusher();
	_esperActor = HACKEDInGameManager->GetEsper();
}

void ACharacterSelector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (_esperActor->bSkipTutorial && _crusherActor->bSkipTutorial)
	{

	}



}

void ACharacterSelector::_SkipCheck()
{
	if (bSkip || NetBaseCP->IsSingle()) {
		if (!NetBaseCP->OnServer()) {
			auto networkSystem = NetBaseCP->GetNetworkSystem();
			networkSystem->StopToInit();
		}
		if (NetBaseCP->IsMaster()) {
			_characterSelectWidget->PossessProc();
			_bEndSelect = true;
			bSkip = true;
		}
		OnEndCharacterSelect();
		_RemoveWidget();
	}
}

void ACharacterSelector::Select(bool isEsper)
{
	if(NetBaseCP && NetBaseCP->GetNetworkSystem()) 
		_MasterSelect(NetBaseCP->GetNetworkSystem()->GetCurrentSlot(), isEsper);
	else LOG(Warning, "Something is wrong... NetBaseCP or Network System not exist.");
}

void ACharacterSelector::_MasterSelect(int slot, bool isEsper)
{
	RPC(NetBaseCP, ACharacterSelector, _MasterSelect, ENetRPCType::MASTER, true, slot, isEsper);

	// 타겟 현재 상태
	ESelectState* targetCurrentState;
	if(isEsper) targetCurrentState = &esperState;
	else targetCurrentState = &crusherState;
	CHECK(targetCurrentState);
	
	// 비 타겟
	ESelectState* notTargetCurrentState;
	if(isEsper) notTargetCurrentState = &crusherState;
	else notTargetCurrentState = &esperState;
	CHECK(notTargetCurrentState);

	// 체킹 시 상태
	ESelectState owningState;
	if(slot == 0) owningState = ESelectState::MASTER_SELECTED;
	else owningState = ESelectState::SLAVE_SELECTED;

	if(*targetCurrentState == ESelectState::NOT_SELECTED){
		*targetCurrentState = owningState;
		_RPCSelect(slot, isEsper, true);
		
		if(*notTargetCurrentState == owningState){
			*notTargetCurrentState = ESelectState::NOT_SELECTED;
			_RPCSelect(slot, !isEsper, false);
		}
	}
	else {
		if(*targetCurrentState == owningState){
			*targetCurrentState = ESelectState::NOT_SELECTED;
			_RPCSelect(slot, isEsper, false);
		}
	}
}

void ACharacterSelector::_RPCSelect(int slot, bool isEsper, bool isOn)
{
	RPC(NetBaseCP, ACharacterSelector, _RPCSelect, ENetRPCType::MULTICAST, true, slot, isEsper, isOn);
	CHECK(_characterSelectWidget);
	if(isOn){
		_characterSelectWidget->Select(isEsper, slot);
	}
	else {
		_characterSelectWidget->UnSelect(isEsper, slot);
	}

	// Character Select is done?
	if(NetBaseCP->IsMaster() && 
		crusherState != ESelectState::NOT_SELECTED &&
		esperState != ESelectState::NOT_SELECTED)
	{
		_EndSelect();
	}
}

bool ACharacterSelector::IsEndSelect()
{
	return *_bEndSelect;
}

void ACharacterSelector::_EndSelect()
{
	RPC(NetBaseCP, ACharacterSelector, _EndSelect, ENetRPCType::MULTICAST, true);
	_characterSelectWidget->EndSelect();
	auto controller = GetWorld()->GetFirstPlayerController();
	if(controller) controller->bShowMouseCursor = false;
	_bEndSelect = true;
	GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ACharacterSelector::GameIntroStart, 4.0f, false);
}

void ACharacterSelector::GameIntroStart()
{
	HACKEDInGameManager->GetFadeInOutMaker()->GameIntro(); // 게임 인트로 영상 재생 

	GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ACharacterSelector::GameIntroFadeInOut, 24.0f, false); // 인트로 영상 길에 맞게 타이머를 넣어야함. 
}

void ACharacterSelector::GameIntroFadeInOut()
{
	//HACKEDInGameManager->GetFadeInOutMaker()->FadeInOut(2.0f, 0.0f);
	GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ACharacterSelector::GameIntroFinish, 2.0f, false); // 인트로 영상 길에 맞게 타이머를 넣어야함. 
}

void ACharacterSelector::GameIntroFinish()
{
	HACKEDInGameManager->GetFadeInOutMaker()->GameIntroEnd(); // 게임 인트로 영상 멈추고 에스퍼 크러셔의 스킵여부 체킹 시퀀스
}

void ACharacterSelector::OnTutorialStart()
{
	TutorialSoundComp->SetSound(DashQuestSound);
	TutorialSoundComp->Play();
	LOG_WARNING("%s", *(GetWorld()->GetFirstPlayerController()->GetPawn()->GetName()));
	// 내가 크러셔를 선택했는가?
	if (GetWorld()->GetFirstPlayerController()->GetPawn()->IsA(AHACKED_CRUSHER::StaticClass()))
	{
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ1"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("CQS1");
	}
	// 내가 에스퍼를 선택했는가?
	if(GetWorld()->GetFirstPlayerController()->GetPawn()->IsA(AHACKED_ESPER::StaticClass()))
	{
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ1"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("EQS1");
	}
}