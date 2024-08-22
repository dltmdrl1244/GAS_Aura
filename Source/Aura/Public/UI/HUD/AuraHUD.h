// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAuraUserWidget;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	
protected:
	
private:
	// 체력, 마나, 경험치, 스킬 퀵슬롯 등의 Overlay UI를 만들기 위한 Class
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
	// Overlay Widget에서 필요한 데이터들을 얻을 수 있는 인터페이스 역할을 하는 Widget Controller를 만들기 위한 Class
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// Overlay Widget에서 필요한 데이터들을 얻을 수 있는 인터페이스 역할을 하는 Widget Controller. private에 존재하며 public getter를 통해 접근
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
};
