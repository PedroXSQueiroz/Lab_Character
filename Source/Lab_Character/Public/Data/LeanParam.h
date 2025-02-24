// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/LabAnimParams.h"

#include "LeanParam.generated.h"

struct FMovementState;

UENUM(BlueprintType)
enum ELeanType : uint8
{
	VELOCITY,
	ACCELERATION,
	DIRECTION
};

/**
 * 
 */
UCLASS()
class LAB_CHARACTER_API ULeanParam : public ULabAnimParams
{
	GENERATED_BODY()

public:

	float GetIntensity(UAnimInstance* anim, EAxis::Type axis, FMovementState movementState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	TEnumAsByte<ELeanType> LeanType;
};



UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API ULeanParamBlendAnimByAxis : public ULeanParam 
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimSequence* PositiveDeviation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	UAnimSequence* NegativeDeviation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float MaxDeviationScale;

	FLeanStateBlendAnim GetState(UAnimInstance* anim, EAxis::Type axis, FMovementState movementState);
};

UCLASS()
class LAB_CHARACTER_API ULeanParamProcedural : public ULeanParam 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FRuntimeFloatCurve DealocationIntensityByDeviation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	TEnumAsByte<EAxis::Type> ReferenceAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	TEnumAsByte<EAxis::Type> TargetAxis;

	FLeanStateProcedural GetState(UAnimInstance* anim, FMovementState movementState);

	virtual TArray<FLeanBone> CalculateLean(float intensity, EAxis::Type axisReference) PURE_VIRTUAL(TEXT("NOT IMPLEMENTED YET"), return TArray<FLeanBone>(););
};


UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API ULeanParamDealocation: public ULeanParamProcedural
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float MaxDealoaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName Effector;

	virtual TArray<FLeanBone> CalculateLean(float intensity, EAxis::Type axisTarget) override;
};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FLeanBone 
{
	GENERATED_BODY()

public:

	FLeanBone()
		: Name("")
		, Transform(FTransform()){
	}

	FLeanBone(
		FName name,
		FTransform transf
	)
		: Name(name)
		, Transform(FTransform(transf)) {
	}

	UPROPERTY(BlueprintReadOnly)
	FName Name;

	UPROPERTY(BlueprintReadOnly)
	FTransform Transform;
};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FLeanStateProcedural : public FLabAninmState
{
	GENERATED_BODY()

public:

	FLeanStateProcedural()
	:	FLabAninmState(true),
		TransformPerBone(TArray<FLeanBone>())
	{}

	FLeanStateProcedural(TArray<FLeanBone> transforms)
	:	FLabAninmState(false),
		TransformPerBone(transforms)
	{}

	UPROPERTY(BlueprintReadOnly)
	TArray<FLeanBone> TransformPerBone;
};

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FLeanStateBlendAnim : public FLabAninmState
{
	GENERATED_BODY()

public:

	FLeanStateBlendAnim(): 
		FLabAninmState(true)
	,	Intensity(0)
	,	AnimToBlend(NULL){
		
	};

	FLeanStateBlendAnim(
		UAnimSequence* defaultAnim
	) : FLabAninmState(true)
	,	AnimToBlend(defaultAnim)
	,	Intensity(0)
	{
	}

	FLeanStateBlendAnim(
		float intensity,
		UAnimSequence* anim
	):	FLabAninmState(false)
	,	Intensity(intensity)
	,	AnimToBlend(anim)
	{}

	UPROPERTY(BlueprintReadOnly)
	float Intensity;

	UPROPERTY(BlueprintReadOnly)
	UAnimSequence* AnimToBlend;

};
