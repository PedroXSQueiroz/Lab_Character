// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/IKRootParams.h"

#include "Kismet/KismetMathLibrary.h"

#include "Data/IKParams.h"
#include "Components/AnimInstance/BaseAnimInstance.h"

#pragma optimize("", off)
FIKRootState UIKRootParams::GetCurrentRootTransform(UBaseAnimInstance* anim)
{
	TArray<FIKState> iks = anim->GetCurrentIKStates();

	/*FVector direction = FVector::ZeroVector;
	float greaterDistance = 0;*/

	USkeletalMeshComponent* body = anim->GetOwningComponent();
	float maxExceedingDistance = 0;


	for (FIKState currentIK : iks) 
	{
		const FName currentIKName = currentIK.Name;

		if (currentIKName.IsNone()) 
		{
			continue;
		}
		
		UIKParams* currentIKParamInstance = anim->IKParams.FilterByPredicate([currentIKName](const UIKParams* params) {
			return params->Name == currentIKName;
		})[0];
		
		/*UIKParamsByTrace* currentIKParam = Cast<UIKParamsByTrace>(currentIKParamInstance);
		direction += currentIKParam->TraceDirection;*/


		if (currentIK.DealocationFromCenter > currentIKParamInstance->MaxDistanceFromRoot)
		{
			float exceeding = currentIK.DealocationFromCenter - currentIKParamInstance->MaxDistanceFromRoot;

			if (exceeding > maxExceedingDistance) 
			{
				maxExceedingDistance = exceeding;
			}
		}
	}

	/*FVector finalDealocation = FVector::ZeroVector;

	for (FVector dealocation : dealocations) 
	{
		finalDealocation += dealocation;
	}
	
	finalDealocation /= dealocations.Num();*/

	FVector finalDealocation = this->DealocationDirection * maxExceedingDistance;

	if (this->Debug) 
	{
		DrawDebugSphere(
			anim->GetWorld(),
			anim->GetOwningComponent()->GetSocketLocation(this->BoneName) + finalDealocation,
			12,
			12,
			FColor::Red
		);

		UE_LOG(LogTemp, Log, TEXT("Root: %s => %s"), *this->Name.ToString(), *finalDealocation.ToString())
	}

	return FIKRootState(
		this->BoneName,
		this->Name,
		FTransform(
			finalDealocation
		)
	);
}
#pragma optimize("", on)
