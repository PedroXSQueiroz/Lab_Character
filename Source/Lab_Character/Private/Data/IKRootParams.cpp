// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/IKRootParams.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "Data/IKParams.h"
#include "Components/AnimInstance/BaseAnimInstance.h"

#pragma optimize("", off)
FIKRootState UIKRootParams::GetCurrentRootTransform(UBaseAnimInstance* anim, float DeltaTime)
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
		
		TArray<UIKParams*> rootParamsFound = anim->IKParams.FilterByPredicate([currentIKName](const UIKParams* params) {
			return params && params->Name == currentIKName;
		});

		if (rootParamsFound.Num() != 1) 
		{
			return FIKRootState();
		}

		UIKParams* currentIKParamInstance = rootParamsFound[0];
		
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
	FIKRootState* previewsState = anim->GetCurrentRootIKStates().FindByPredicate([&](FIKRootState state) { return state.RootParamName.IsEqual(this->Name); });
	FVector currentSpringVelocity = FVector::ZeroVector;

	if (previewsState) 
	{
		FVector currentLocation = previewsState->Transform.GetLocation();
		FVector previewsLocationRate = this->DealocationDirection;
		currentSpringVelocity = previewsState->SpringLerpVelocity;
		
		/*FMath::SpringDamperSmoothing<FVector>(
			currentLocation,
			currentSpringVelocity,
			finalDealocation * this->DealocationDirection,
			FVector::ZeroVector,
			DeltaTime,
			this->SmoothTime,
			this->SmoothRatio
		);*/
		//finalDealocation = currentLocation;

		float TargetScalar = FVector::DotProduct(finalDealocation, DealocationDirection);
		float CurrentScalar = FVector::DotProduct(currentLocation, DealocationDirection);
		float VelocityScalar = FVector::DotProduct(currentSpringVelocity, DealocationDirection);

		FMath::SpringDamperSmoothing<float>(
			CurrentScalar,
			VelocityScalar,
			maxExceedingDistance,
			0.f,
			DeltaTime,
			SmoothTime,
			SmoothRatio
		);

		finalDealocation = DealocationDirection * CurrentScalar;
		currentSpringVelocity = DealocationDirection * VelocityScalar;
	}
	
	if (currentSpringVelocity.SizeSquared() < 0.01f &&
		finalDealocation.SizeSquared() < 0.01f)
	{
		finalDealocation = FVector::ZeroVector;
		currentSpringVelocity = FVector::ZeroVector;
	}

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
		), currentSpringVelocity
	);
}
#pragma optimize("", on)
