// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/LabAnimParams.h"

#include "IKParams.generated.h"

class UWeightProvider;
class UBaseAnimInstance;

/**
 * 
 */
UCLASS()
class LAB_CHARACTER_API UIKParams : public ULabAnimParams
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Params")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName Effector;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Params", meta = (AllowInherited = true))
	UWeightProvider* WeightProvider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FVector MinAnglePivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FVector MaxAnglePivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName PivotBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float MaxDistanceFromRoot;

	virtual FTransform GetEffectorTransform(
			UAnimInstance* anim
		,	FTransform previewsTransform
		,	float weight
		,	float& distance
		,	FRotator& effectorRotation
	) PURE_VIRTUAL(TEXT("NOT IMPLEMENTED YET"), return FTransform(); );

	FIKState GetIKState(UBaseAnimInstance* anim);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float MaxNormalDeviation;

};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UIKParamsByTrace : public UIKParams 
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FVector TraceDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float StartPointDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float EndPointDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float SweepRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	bool DebugTraces {false};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Params", meta = (AllowInherited = true))
	UIKParamByTraceCustomRotation* RotationCalc;

	virtual FTransform GetEffectorTransform(
			UAnimInstance* anim
		,	FTransform previewsTransform
		,	float weight
		,	float& distance
		,	FRotator& effectorRotation
	) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UIKParamByTraceCustomRotation: public UObject
{
	GENERATED_BODY()

public:

	virtual FRotator GetRotationFromTraceHit(FHitResult hit, UAnimInstance* anim, UIKParamsByTrace* traceParams) PURE_VIRTUAL(TEXT("NOT IMPLEMENTED YET"), return FRotator(); );
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UIKCustomRotationHorizontal : public UIKParamByTraceCustomRotation
{

	GENERATED_BODY()

public:

	virtual FRotator GetRotationFromTraceHit(FHitResult hit, UAnimInstance* anim, UIKParamsByTrace* traceParams) override;
};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FIKState
{
	GENERATED_BODY()

public:

	FIKState():IsNone(true) {}

	FIKState(
			FName name
		,	FName root
		,	FName effector
		,	FTransform transform
		,	FTransform relativeTransform
		,	FVector minPivotAngle = FVector::ZeroVector
		,	FVector maxPivotAngle = FVector::ZeroVector
		,	FName pivotBoneName = FName("")
		,	float dealocation = 0
		):	Name(name),
			Root(root),
			Effector(effector),
			EffectorTransform(transform),
			EffectorRelativeTransform(relativeTransform),
			MinPivotAngle(minPivotAngle),
			MaxPivotAngle(maxPivotAngle),
			PivotBoneName(pivotBoneName),
			DealocationFromCenter(dealocation),
			IsNone(false)
	{}

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FName Root;

	UPROPERTY()
	FName Effector;

	UPROPERTY()
	FTransform EffectorTransform;

	UPROPERTY()
	FTransform EffectorRelativeTransform;

	UPROPERTY()
	FVector MinPivotAngle;

	UPROPERTY()
	FVector MaxPivotAngle;

	UPROPERTY()
	FName PivotBoneName;

	UPROPERTY()
	float DealocationFromCenter;

	bool GetIsNone() 
	{
		return this->IsNone;
	}

private:

	bool IsNone;
};
