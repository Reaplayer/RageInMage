// Copyright Reaplays


#include "AbilitySystem/Abilities/RageInMageSummonGameplayAbility.h"

TArray<FVector> URageInMageSummonGameplayAbility::GetSummonLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpreadAngle / SummonCount;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(SpawnSpreadAngle / 2, FVector::UpVector);
	
	TArray<FVector> SummonLocations;
	for (int32 i = 0; i < SummonCount; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			ChosenSpawnLocation = HitResult.ImpactPoint;
		}
		SummonLocations.Add(ChosenSpawnLocation);
		
	}
	return SummonLocations;
}
