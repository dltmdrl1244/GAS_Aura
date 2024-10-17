// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AuraBeamSpell.h"

#include "GameFramework/Character.h"

void UAuraBeamSpell::StoreCursorDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		CursorHitLocation = HitResult.ImpactPoint;
		CursorHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}
