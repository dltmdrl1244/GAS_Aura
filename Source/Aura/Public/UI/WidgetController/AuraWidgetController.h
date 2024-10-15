// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

class UAbilitySystemComponent;
class UAttributeSet;
class AAuraPlayerController;
class AAuraPlayerState;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UAbilityInfo;

/* WidgetController 객체를 생성 및 관리함에서의 편리를 위한 구조체
 * WidgetController는 PC, PS, ASC, AS 4가지 클래스 포인터가 필요하고 이를 하나로 묶는 역할
 */
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS,
		UAbilitySystemComponent* ASC, UAttributeSet* AS) :
	PlayerController(PC),
	PlayerState(PS),
	AbilitySystemComponent(ASC),
	AttributeSet(AS)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;

	void BroadcastAbilityInfo();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	// WidgetController에서 필요한 4가지 데이터 소스 종류. SetWidgetControllerParams() 에서 set 된다.
	UPROPERTY(BlueprintReadOnly, Category = WidgetController)
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category = WidgetController)
	TObjectPtr<APlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly, Category = WidgetController)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, Category = WidgetController)
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAuraPlayerController> AuraPlayerController;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAuraPlayerState> AuraPlayerState;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAuraAttributeSet> AuraAttributeSet;
	
	AAuraPlayerController* GetAuraPC();
	AAuraPlayerState* GetAuraPS();
	UAuraAbilitySystemComponent* GetAuraASC();
	UAuraAttributeSet* GetAuraAS();
};
