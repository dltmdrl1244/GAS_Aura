// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreCursorDataInfo(const FHitResult& HitResult);
	
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector CursorHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> CursorHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
}; 
