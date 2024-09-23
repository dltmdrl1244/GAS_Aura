// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

// 캐릭터 타입을 정의
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	// Set of abilities that each character class should start the game with
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
		
	UPROPERTY(EditDefaultsOnly, Category = "Class Common Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Class Common Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	// 모든 캐릭터들이 공통으로 가지고 있는 gameplay ability
	// AuraAbilitySystemLibrary-> OOO 에서 실행
	UPROPERTY(EditDefaultsOnly, Category = "Class Common Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	// Attribute 계수
	UPROPERTY(EditDefaultsOnly, Category = "Class Common Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
	
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
