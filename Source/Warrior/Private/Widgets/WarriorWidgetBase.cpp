// Learning Project


#include "Widgets/WarriorWidgetBase.h"

#include "Components/UI/EnemyUIComponent.h"
#include "Interfaces/PawnUIInterface.h"

void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
		
	}
}

void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();
		checkf(EnemyUIComponent, TEXT("Failed to get an EnemyUIComponent from %s"), *OwningEnemyActor->GetActorNameOrLabel());
        BP_OnOwningEnemyUiComponentInitialized(EnemyUIComponent);
	}
}
