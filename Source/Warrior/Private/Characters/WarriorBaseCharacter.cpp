// Learning Project


#include "Characters/WarriorBaseCharacter.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

}

UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}

UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* AWarriorBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

void AWarriorBaseCharacter::PossessedBy(AController* NewController) 
{
	Super::PossessedBy(NewController);
	if (WarriorAbilitySystemComponent)
	{
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);
 		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forget to assign start up data to %s"), *GetName());
	}
}


