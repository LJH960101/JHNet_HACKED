// Fill out your copyright notice in the Description page of Project Settings.
#include "Interactor.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "Components/WidgetComponent.h"

// Sets default values
AInteractor::AInteractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	box = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX COLLISION"));
	interactionWG = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTION WIDGET COMPONENT"));
	RootComponent = box;
	interactionWG->SetupAttachment(box);
	interactionWG->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> WG_INTERACTION(TEXT("WidgetBlueprint'/Game/Resources/UI/Interaction/WG_Interaction.WG_Interaction_C'"));
	if (WG_INTERACTION.Succeeded())
	{
		interactionWG->SetWidgetClass(WG_INTERACTION.Class);
		interactionWG->SetDrawSize(FVector2D(158.0f, 58.0f));
	}	
}

void AInteractor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	box->OnComponentBeginOverlap.AddDynamic(this, &AInteractor::OnOverlapBegin);
	box->OnComponentEndOverlap.AddDynamic(this, &AInteractor::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AInteractor::BeginPlay()
{
	Super::BeginPlay();
	CloseWidget();
}

// Called every frame
void AInteractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsInteracted) {
		if (bIsAnyEntered)
			ShowWidget();
		else
			CloseWidget();
	}
	else
	{
		CloseWidget();
	}
}

void AInteractor::Interact(AActor* caller)
{

}

void AInteractor::ShowWidget()
{
	interactionWG->SetVisibility(true);
}

void AInteractor::CloseWidget()
{

	interactionWG->SetVisibility(false);
}

void AInteractor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HLOG("Overlap %s",*OtherActor->GetName());
	
	if (OtherActor->IsA(AHACKEDCharacter::StaticClass())) {
		AHACKEDCharacter* hackedCharacter = Cast<AHACKEDCharacter>(OtherActor);
		if(hackedCharacter && hackedCharacter->NetBaseCP->HasAuthority()) bIsAnyEntered = true;
	}
}

void AInteractor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AHACKEDCharacter::StaticClass())) {
		AHACKEDCharacter* hackedCharacter = Cast<AHACKEDCharacter>(OtherActor);
		if (hackedCharacter && hackedCharacter->NetBaseCP->HasAuthority()) bIsAnyEntered = false;
	}
}

