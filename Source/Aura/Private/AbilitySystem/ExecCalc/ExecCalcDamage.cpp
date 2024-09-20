// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalcDamage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

UExecCalcDamage::UExecCalcDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
}

void UExecCalcDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatarActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatarActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatarActor);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatarActor);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParam;
	EvalParam.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParam.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get Damage Set by caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	// Capture BlockChance on Target
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvalParam, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	// if block, half damage
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	Damage = bBlocked ? Damage / 2.f : Damage;
	
	// Custom Effect Context에서 새로 만든 bBlockedHit에 값 할당
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	// Armor ignores a percentage of Target's armor
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParam, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);
	
	const UCharacterClassInfo* SourceClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatarActor);
	const UCharacterClassInfo* TargetClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(TargetAvatarActor);
	const FRealCurve* ArmorPenetrationCurve = SourceClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const FRealCurve* EffectiveArmorCurve = SourceClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetCharacterLevel());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel());
	
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvalParam, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const float EffectiveArmor = TargetArmor * (100 - ArmorPenetrationCoefficient) / 100.f;
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// Critical Hit
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvalParam, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvalParam, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvalParam, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	const FRealCurve* CriticalHitResistanceCurve = TargetClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetCharacterLevel());

	// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	// const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	const bool bCriticalHit = FMath::RandRange(1, 100) < SourceCriticalHitChance;

	// Custom Effect Context에서 새로 만든 bIsCriticalHit에 값 할당
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	
	// Double damage plus a bonus if critical hit
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
	
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
