// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/TargetDataUnderCursor.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

UTargetDataUnderCursor* UTargetDataUnderCursor::CreateTargetDataUnderCursor(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderCursor* MyObj = NewAbilityTask<UTargetDataUnderCursor>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderCursor::Activate()
{
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled()) // Client side
	{
		SendCursorData();
	}
	else // Server Side
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
		.AddUObject(this, &UTargetDataUnderCursor::OnTargetDataReplicatedCallback);
		
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

// 클라이언트 -> 서버 방향으로 CursorHit 결과 데이터를 전달하는 함수
void UTargetDataUnderCursor::SendCursorData()
{
	// FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Ability->GetCurrentActorInfo()->PlayerController.Get());
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = AuraPC->GetCursorHit();
	
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);

	// Target Data를 서버에 전달하는 ServerRPC
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(), // Prediction Key 생성
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
	
	// This should be called prior to broadcasting delegates back into the ability graph.
	// This makes sure the ability is still active.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderCursor::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
