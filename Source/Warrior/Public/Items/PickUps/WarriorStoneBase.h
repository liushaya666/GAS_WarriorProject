// Learning Project

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUps/WarriorPickUpBase.h"
#include "WarriorStoneBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorStoneBase : public AWarriorPickUpBase
{
	GENERATED_BODY()
public:
	void Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel);
protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Stone Consume"))
	void BP_OnStoneConsumed();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
};
