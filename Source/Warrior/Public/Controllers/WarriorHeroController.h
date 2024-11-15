// Learning Project

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "WarriorHeroController.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AWarriorHeroController();
	//~ Begin GenericTeamAgent Interface 
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End GenericTeamAgent Interface 
private:
	FGenericTeamId HeroTeamId;
};
