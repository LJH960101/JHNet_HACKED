// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HackedCharacterAnimInstance.generated.h"

UCLASS()
class HACKED_API UHackedCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	class AHACKEDCharacter* player;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instance, Meta = (AllowPrivateAccess = "true"))
	float currentSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instance, Meta = (AllowPrivateAccess = "true"))
	float currentForwardSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instance, Meta = (AllowPrivateAccess = "true"))
	float currentRightSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instance, Meta = (AllowPrivateAccess = "true"))
	bool bOnMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instance, Meta = (AllowPrivateAccess = "true"))
	bool bOnJump;

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
