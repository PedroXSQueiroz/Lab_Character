// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Data/LabAnimParams.h"


#include "IKRootParams.generated.h"

class UBaseAnimInstance;

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FIKRootState 
{
	GENERATED_BODY()

public:

	FIKRootState() {}

	FIKRootState(
		FName boneName,
		FName rootParamName,
		FTransform transform,
		FVector springVelocity = FVector::ZeroVector
	)	: BoneName(boneName)
		, RootParamName(rootParamName)
		, Transform(transform)
	{
	}

	UPROPERTY()
	FName BoneName;

	UPROPERTY()
	FName RootParamName;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector SpringLerpVelocity;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LAB_CHARACTER_API UIKRootParams : public ULabAnimParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	TArray<FName> IKNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	bool Debug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FVector DealocationDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float SmoothTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float SmoothRatio;

	FIKRootState GetCurrentRootTransform(UBaseAnimInstance* anim, float DeltaTime);

	
};
