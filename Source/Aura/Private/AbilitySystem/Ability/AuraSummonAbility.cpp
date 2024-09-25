// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	TArray<FVector> OutLocations;
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = NumMinions == 1 ? SpawnSpread / 2.f : SpawnSpread / (NumMinions - 1);

	const FVector LeftSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	// UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + LeftSpread * MaxSpawnDistance, 10.f, FLinearColor::Blue, 3.f);

	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		const FVector RandomDirection = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, RandomDirection + FVector(0.f, 0.f, 400.f), RandomDirection - FVector(0.f, 0.f, 400.f), ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			OutLocations.Add(Hit.ImpactPoint);
		}
	}
	
	const FVector RightSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	// UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + RightSpread * MaxSpawnDistance, 10.f, FLinearColor::Green, 3.f);
	
	return OutLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	int32 RandomIdx = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[RandomIdx];
}
