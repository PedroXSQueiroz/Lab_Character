// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include <Data/IKParams.h>
#include <Data/IKRootParams.h>
#include <Data/LeanParam.h>

#include "BaseAnimInstance.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct LAB_CHARACTER_API FMovementState 
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FRotator DirectionDeviation;

	UPROPERTY(BlueprintReadOnly)
	FVector CurrentVelocity;

	UPROPERTY(BlueprintReadOnly)
	FVector CurrentAcceleration;

	UPROPERTY(BlueprintReadOnly)
	float VelocityScale;

};

/**
 * 
 */
UCLASS()
class LAB_CHARACTER_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|IKs")
	float MaxVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|IKs", meta = (AllowInherited = true))
	TArray<UIKParams*> IKParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|IKs", meta = (AllowInherited = true))
	TArray<UIKRootParams*> IKRootParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Lean")
	UAnimSequence* DefaultLeanAnim;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|Lean", meta = (AllowInherited = true))
	TMap<TEnumAsByte<EAxis::Type>, ULeanParamBlendAnimByAxis*> LeanByBlendParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|Lean", meta = (AllowInherited = true))
	TArray<ULeanParamProcedural*> ProceduralLeans;

	UFUNCTION(BlueprintCallable)
	void InitIKParams();

	UFUNCTION(BlueprintCallable)
	void UpdateMovementState();

	UFUNCTION(BlueprintCallable)
	TArray<FIKState> UpdateCurrentIKsStates();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FIKState> GetCurrentIKStates();

	FIKState GetCurrentIKStateByName(FName name, bool& found);

	UFUNCTION(BlueprintCallable)
	TArray<FIKRootState> UpdateCurrentIKRootStates();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FIKRootState> GetCurrentRootIKStates();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLeanStateBlendAnim GetLeanByBlendAnimByAxis(EAxis::Type axis);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FLeanStateProcedural> GetLeanProcStates();

	UPROPERTY(BlueprintReadOnly)
	FMovementState MovementState;

	void ClearCaches();

	void SetDesiredForwardRotation(FRotator rotation);

private:

	FRotator GetCurrentDeviation();

	UPROPERTY()
	FRotator DesiredForwardRotation;

	UPROPERTY()
	TMap<FName, FIKState> IKStatesCache;

	UPROPERTY()
	TMap<FName, FIKRootState> IKRootsStatesCache;


};
