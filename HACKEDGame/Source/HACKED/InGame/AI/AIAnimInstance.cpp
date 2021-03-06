// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "AIAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		UActorComponent* cp = Pawn->GetComponentByClass(UCharacterMovementComponent::StaticClass());
		if (cp) {
			UCharacterMovementComponent* movementCP = Cast<UCharacterMovementComponent>(cp);
			MaxPawnSpeed = movementCP->GetMaxSpeed();
			CurrentPawnSpeedRate = CurrentPawnSpeed / MaxPawnSpeed;
		}
	}

}