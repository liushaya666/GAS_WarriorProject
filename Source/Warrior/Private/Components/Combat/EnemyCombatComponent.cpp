// Learning Project


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	OverlappedActors.AddUnique(HitActor);
	
	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WarriorGameplayTags::Player_Status_Blocking);
	const bool bIsMyAttackingUnblockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), WarriorGameplayTags::Enemy_Status_Unblockable);
	if (bIsPlayerBlocking && !bIsMyAttackingUnblockable)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsVaildBlock(GetOwningPawn(), HitActor);
	}
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor,WarriorGameplayTags::Player_Event_SuccessfulBlock, EventData);
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), WarriorGameplayTags::Shared_Event_MeleeHit, EventData);
	}
}
