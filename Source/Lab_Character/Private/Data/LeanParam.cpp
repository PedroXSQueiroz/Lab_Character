// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/LeanParam.h"

#include "Components/AnimInstance/BaseAnimInstance.h"

#include "GameFramework/Character.h"
#include <Kismet/KismetMathLibrary.h>

float ULeanParam::GetIntensity(UAnimInstance* anim, EAxis::Type axis, FMovementState movementState)
{
	switch (this->LeanType) 
	{
		case ELeanType::DIRECTION_STOPPED:
		switch (axis)
		{
			case EAxis::Type::X: return (movementState.DirectionDeviation.Roll / 180);
			case EAxis::Type::Y: return (movementState.DirectionDeviation.Pitch / 180);
			case EAxis::Type::Z: return (movementState.DirectionDeviation.Yaw / 180);
		}
		break;
	
		case ELeanType::DIRECTION :
			switch (axis) 
			{
				case EAxis::Type::X: return ( movementState.DirectionDeviation.Roll / 180 ) * movementState.VelocityScale;
				case EAxis::Type::Y: return ( movementState.DirectionDeviation.Pitch / 180 ) * movementState.VelocityScale;
				case EAxis::Type::Z: return ( movementState.DirectionDeviation.Yaw/ 180 ) * movementState.VelocityScale;
			}
		break;

		case ELeanType::VELOCITY :
			return movementState.VelocityScale;
			break;
	}
	
	return 0;
}

FLeanStateBlendAnim ULeanParamBlendAnimByAxis::GetState(UAnimInstance* anim, EAxis::Type axis, FMovementState movementState)
{
	float intensity = this->GetIntensity(anim, axis, movementState);

	float intensityScaled = FMath::Abs(intensity);
	float finalIntensity = FMath::Clamp(intensityScaled, 0, this->MaxDeviationScale);

	return FLeanStateBlendAnim(
		finalIntensity
	,	intensity > 0 ? this->PositiveDeviation : this->NegativeDeviation
	);
}

FLeanStateProcedural ULeanParamProcedural::GetState(UAnimInstance* anim, FMovementState movementState, float weight)
{
	return FLeanStateProcedural(
		this->CalculateLean(
				this->GetIntensity(anim, this->ReferenceAxis, movementState)
			,	this->TargetAxis
		), weight
	);
}

TArray<FLeanBone> ULeanParamDealocation::CalculateLean(float intensity, EAxis::Type axisTarget)
{
	
	FRichCurve* intensityOutCurve = this->DealocationIntensityByDeviation.GetRichCurve();
	
	FVector dealocation = FVector(
		axisTarget == EAxis::Type::X ? intensityOutCurve->Eval(intensity) * this->MaxDealoaction: 0,
		axisTarget == EAxis::Type::Y ? intensityOutCurve->Eval(intensity) * this->MaxDealoaction: 0,
		axisTarget == EAxis::Type::Z ? intensityOutCurve->Eval(intensity) * this->MaxDealoaction: 0
	);
	
	return TArray<FLeanBone> { 
		FLeanBone(this->Effector, FTransform(dealocation))
	};
}

TArray<FLeanBone> ULeanChainParamDealocation::CalculateLean(float intensity, EAxis::Type axisTarget)
{
	UE_LOG(LogTemp, Log, TEXT("CALCULATING LEAN ON CHAIN"))
	
	TArray<FLeanBone> leanResults = TArray<FLeanBone>();

	if (this->Effectors.Num() <= 1) 
	{
		return leanResults;
	}

	FRichCurve* intensityOutCurve = this->DealocationIntensityByDeviation.GetRichCurve();
	FRichCurve* intensityInChainCurve = this->DealocationIntensityOnChain.GetRichCurve();
	
	float effectorOffsetCurve = 1 / ( this->Effectors.Num() - 1 );

	float accumulatedLean = this->Offset;
	


	for (int effectorIndex = 0; effectorIndex < this->Effectors.Num(); effectorIndex++) 
	{
		float currentEffectorOffset = effectorOffsetCurve * effectorIndex;
		
		float currentIntensity = intensityOutCurve->Eval(intensity)* this->MaxDealoaction* intensityInChainCurve->Eval(effectorIndex) -  accumulatedLean;
		
		FRotator dealocation = FRotator(
			axisTarget == EAxis::Type::X ? ( currentIntensity * 180 ): 0,
			axisTarget == EAxis::Type::Y ? ( currentIntensity * 180 ): 0,
			axisTarget == EAxis::Type::Z ? ( currentIntensity * 180 ): 0
		);

		UE_LOG(LogTemp, Log, TEXT("CALCULATING LEAN ON CHAIN => APPLYING %.2f TO BONE %s"), intensity * 180, *this->Effectors[effectorIndex].ToString())

		leanResults.Add(FLeanBone(this->Effectors[effectorIndex], FTransform(dealocation)));
		accumulatedLean += currentIntensity;
	}

	return leanResults;
}
