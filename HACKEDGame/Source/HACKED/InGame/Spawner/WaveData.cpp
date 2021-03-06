// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 
#include "WaveData.h"
#include "InGame/AI/SR_ET/SecurityRobot_ET.h"
#include "InGame/AI/SR_LS/SecurityRobot_LS.h"
#include "InGame/AI/SR_SLS/SecurityRobot_SLS.h"
#include "InGame/AI/SelfDestructAI/SelfDestructAI.h"
#include "InGame/AI/MilitarySoldierAI/MilitarySoldierAI.h"
#include "InGame/AI/MilitaryNamedAI/MilitaryNamedAI.h"
#include "InGame/HACKEDTool.h"

UClass* GetAIClassByType(const EHackedAIType& type)
{
	UClass* retClass;
	switch (type) {
	case EHackedAIType::MS_AI: {
		retClass = LoadClass<AMilitarySoldierAI>(NULL, TEXT("/Game/Blueprint/InGame/AI/BP_MilitarySoliderAI.BP_MilitarySoliderAI_C"));
		return retClass;
	}
	case EHackedAIType::MN_AI: {
		retClass = LoadClass<AMilitaryNamedAI>(NULL, TEXT("/Game/Blueprint/InGame/AI/BP_MilitaryNamedAI.BP_MilitaryNamedAI_C"));
		return retClass;
	}
	case EHackedAIType::SD_AI: {
		retClass = LoadClass<ASelfDestructAI>(NULL, TEXT("/Game/Blueprint/InGame/AI/BP_SelfDestructAI.BP_SelfDestructAI_C"));
		return retClass;
	}
	case EHackedAIType::SR_ET: {
		retClass = LoadClass<ASecurityRobot_ET>(NULL, TEXT("/Game/Blueprint/InGame/AI/BP_SecurityRobot_ET.BP_SecurityRobot_ET_C"));
		return retClass;
	}
	case EHackedAIType::SR_LS: {
		retClass = LoadClass<ASecurityRobot_LS>(NULL, TEXT("/Game/Blueprint/InGame/AI/BP_SecurityRobot_LS.BP_SecurityRobot_LS_C"));
		return retClass;
	}
	case EHackedAIType::SR_SLS: {
		retClass = LoadClass<ASecurityRobot_SLS>(NULL, TEXT("/Game/Blueprint/InGame/AI/BP_SecurityRobot_SLS.BP_SecurityRobot_SLS_C"));
		return retClass;
	}
	default: {
		LOG(Error, "Can't find class..");
		return nullptr;
	}
	}
}
