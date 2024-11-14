// Learning Project

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"


class UWidgetComponent;
class UEnemyUIComponent;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
public:
	AWarriorEnemyCharacter();

	//~ Begin PawnCombat Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End PawnCombat Interface

	//~ Begin PawnUI Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;
	//~ End PawnUI Interface
protected:
	virtual void BeginPlay() override;
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "UI")
	UEnemyUIComponent* EnemyUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "UI")
	UWidgetComponent* EnemyHealthWidgetComponent;
private:
	void InitEnemyStartUpData();
public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent;}
};
