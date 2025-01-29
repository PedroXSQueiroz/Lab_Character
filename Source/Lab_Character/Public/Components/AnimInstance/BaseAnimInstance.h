// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include <Data/IKParams.h>
#include <Data/IKRootParams.h>
#include <Data/LeanParam.h>
#include <Data/TurnInPlaceAnim.h>

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

	//----------------------------------------------------------------------
	//COMMON
	//----------------------------------------------------------------------

public:

	void ClearCaches();
	
	//----------------------------------------------------------------------
	//MOVEMENT
	//----------------------------------------------------------------------

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|IKs")
	float MaxVelocity;
	
	UFUNCTION(BlueprintCallable)
	void UpdateMovementState();
	
	UPROPERTY(BlueprintReadOnly)
	FMovementState MovementState;
	
	void SetDesiredForwardRotation(FRotator rotation);

private:

	FRotator GetCurrentDeviation();

	UPROPERTY()
	FRotator DesiredForwardRotation;
	
	//----------------------------------------------------------------------
	//IK
	//----------------------------------------------------------------------

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|IKs", meta = (AllowInherited = true))
	TArray<UIKParams*> IKParams;
	
	UFUNCTION(BlueprintCallable)
	void InitIKParams();

	UFUNCTION(BlueprintCallable)
	TArray<FIKState> UpdateCurrentIKsStates();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FIKState> GetCurrentIKStates();
	
	FIKState GetCurrentIKStateByName(FName name, bool& found);

private:

	UPROPERTY()
	TMap<FName, FIKState> IKStatesCache;
	
	//----------------------------------------------------------------------
	//IK ROOT
	//----------------------------------------------------------------------

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|IKs", meta = (AllowInherited = true))
	TArray<UIKRootParams*> IKRootParams;
	
	UFUNCTION(BlueprintCallable)
	TArray<FIKRootState> UpdateCurrentIKRootStates();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FIKRootState> GetCurrentRootIKStates();

private:

	UPROPERTY()
	TMap<FName, FIKRootState> IKRootsStatesCache;
	
	//----------------------------------------------------------------------
	//LEAN
	//----------------------------------------------------------------------

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Lean")
	UAnimSequence* DefaultLeanAnim;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|Lean", meta = (AllowInherited = true))
	TMap<TEnumAsByte<EAxis::Type>, ULeanParamBlendAnimByAxis*> LeanByBlendParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|Lean", meta = (AllowInherited = true))
	TArray<ULeanParamProcedural*> ProceduralLeans;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLeanStateBlendAnim GetLeanByBlendAnimByAxis(EAxis::Type axis);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FLeanStateProcedural> GetLeanProcStates();
	
	//----------------------------------------------------------------------
	//TURN IN PLACE
	//----------------------------------------------------------------------

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|TurnInPlace")
	UAnimSequence* FallbackTurnInPlace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|TurnInPlace", meta = (AllowInherited = true))
	TArray<UTurnInPlaceParams*> TurnInPlaceAnims;

	UFUNCTION(BlueprintCallable)
	void UpdateTurnInPlace(float deltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTurnInPlaceState GetTurnInPlaceByAxis(EAxis::Type axis, FRotator deviation, float velocity);

	UPROPERTY(BlueprintReadWrite)
	bool IsTurning;
	
	UPROPERTY(BlueprintReadOnly)
	FTurnInPlaceState CurrentTurningState = FTurnInPlaceState();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentTurningInPlaceWeight();

	UFUNCTION(BlueprintCallable)
	void SetIsTurning(bool turning);

private:

	UPROPERTY()
	FRotator InitialTurningDirection;

	UPROPERTY()
	float TurningProgression = 0;

	UPROPERTY()
	float CurrentTurningInPlaceWeight = 0;

};
