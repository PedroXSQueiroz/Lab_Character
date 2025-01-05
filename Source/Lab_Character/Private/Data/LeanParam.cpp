// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/LeanParam.h"

#include "Components/AnimInstance/BaseAnimInstance.h"

#include "GameFramework/Character.h"
#include <Kismet/KismetMathLibrary.h>

float ULeanParam::GetIntensity(UAnimInstance* anim, EAxis::Type axis, FMovementState movementState)
{
	switch (this->LeanType) 
	{
		case ELeanType::DIRECTION :
			switch (axis) 
			{
				case EAxis::Type::X: return movementState.DirectionDeviation.Roll / 180;
				case EAxis::Type::Y: return movementState.DirectionDeviation.Pitch / 180;
				case EAxis::Type::Z: return movementState.DirectionDeviation.Yaw/ 180;
			}
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

FLeanStateProcedural ULeanParamProcedural::GetState(UAnimInstance* anim, FMovementState movementState)
{
	return FLeanStateProcedural(
		this->CalculateLean(
				this->GetIntensity(anim, this->ReferenceAxis, movementState)
			,	this->TargetAxis
		)
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