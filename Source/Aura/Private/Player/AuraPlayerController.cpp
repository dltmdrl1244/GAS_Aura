// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/HighlightInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/*
	 * 커서로부터 트레이스
	 * 1) LastActor == null && ThisActor == null
	 *		-> 아무것도 하지 않음
	 * 2) LastActor == null && ThisActor != null
	 *		-> 처음으로 액터에 호버된 것이므로 Highlight Actor
	 * 3) LastActor != null && ThisActor == null
	 *		-> 더이상 호버하지 않으므로 UnHighlight Actor
	 * 4) LastActor != null && ThisActor != null && LastActor != ThisActor
	 *		-> 호버 대상이 바뀌었으므로 UnHighlight LastActor, Highlight ThisActor
	 * 5) LastActor != null && ThisActor != null && LastActor == ThisActor
	 *		-> 이미 호버한 대상을 계속 호버하고 있으므로 아무것도 하지 않음
	 */
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// case 2
			ThisActor->HighlightActor();
		}
		else
		{
			// case 1
		}
	}
	else
	{
		if (ThisActor == nullptr)
		{
			// case 3
			LastActor->UnHighlightActor();
		}
		else if (ThisActor != nullptr && LastActor != ThisActor)
		{
			// case 4
			LastActor->UnHighlightActor();
			ThisActor->HighlightActor();
		}
		else
		{
			// case 5
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraConext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraConext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	}
	
}

void AAuraPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControllerPawn = GetPawn())
	{
		// add movement 
		ControllerPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControllerPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}
