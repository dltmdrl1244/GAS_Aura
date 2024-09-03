// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 * Sigleton containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() {return GameplayTags;}
 static void InitializeNativeGameplayTags();

 FGameplayTag Attributes_Secondary_Armor;
 
private:
 // Singleton instance
 static FAuraGameplayTags GameplayTags;
};