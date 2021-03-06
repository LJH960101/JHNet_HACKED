// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "HackedCharacterAnimInstance.h"
#include "HACKEDCharacter.h"

void UHackedCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	player = Cast<AHACKEDCharacter>(TryGetPawnOwner());
	currentSpeedRate = 0.0f;
	bOnMove = false;
	bOnJump = false;
	currentForwardSpeedRate = 0.0f;
	currentRightSpeedRate = 0.0f;
}

void UHackedCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (player) {
		currentSpeedRate = player->currentSpeedRate;
		bOnMove = player->bOnMove;
		bOnJump = player->OnJump();
		currentForwardSpeedRate = player->GetForwardSpeedRate();
		currentRightSpeedRate = player->GetRightSpeedRate();
	}
	else {
		LOG(Warning, "Player not exist.");
	}
}
