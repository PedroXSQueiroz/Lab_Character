// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/WeightProvider.h"

#include "Components/AnimInstance/BaseAnimInstance.h"

void UWeightProvider::Init(UBaseAnimInstance* parentAnimInstance)
{
	this->ParentAnimInstance = parentAnimInstance;
}

float UWeightByAnimCurveProvider::GetCurrentWeight()
{
	if (this->CurveName.IsNone() || !this->ParentAnimInstance)
	{
		return 0;
	}
	
	float weight = 0;

	this->ParentAnimInstance->GetCurveValue(this->CurveName, weight);
	return weight;
}