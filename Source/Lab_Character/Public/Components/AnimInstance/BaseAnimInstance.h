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

	virtual void PostInitProperties() override;

	void SetParamEnabled(FName paramName, bool enabled);

protected:

	template<typename Param>
	bool SetParamEnabledOnList(TArray<Param*> params, FName paramName, bool enabled);

	template<typename Param>
	Param* DuplicateParam(Param* param);

	template<typename Param>
	TArray<Param*> DuplicateParams(TArray<Param*> param);


	template<typename Param, typename Key>	
	TMap<Key, Param*> DuplicateParams(TMap<Key, Param*> param);
	
	template<typename Param>
	void CleanParams(TArray<Param*> param);

	template<typename Param, typename Key>
	void CleanParams(TMap<Key, Param*> param);

public:

	void ClearCaches();
	
	//----------------------------------------------------------------------
	//MOVEMENT
	//----------------------------------------------------------------------

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Movement")
	float MaxVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Movement")
	float DeviationLerp { 1 };
	
	UFUNCTION(BlueprintCallable)
	void UpdateMovementState();
	
	UPROPERTY(BlueprintReadOnly)
	FMovementState MovementState;
	
	UFUNCTION(BlueprintCallable)
	void SetDesiredForwardRotation(FRotator rotation);

	UFUNCTION(BlueprintCallable)
	void DesableDesiredForwardRotation();

private:

	FRotator GetCurrentDeviation();

	UPROPERTY()
	FRotator DesiredForwardRotation;

	UPROPERTY()
	bool bUseDesiredForwardRotation;

	//----------------------------------------------------------------------
	//TRANSITION
	//----------------------------------------------------------------------
	
public:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Movement")
	bool IsTransiting;

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

	UFUNCTION(BlueprintCallable)
	TArray<FLeanStateProcedural> UpdateLeanProcStates();

private:

	UPROPERTY()
	TMap<FName, FLeanStateProcedural> ProcLeanStatesCache;
	
	//----------------------------------------------------------------------
	//TURN IN PLACE
	//----------------------------------------------------------------------

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|TurnInPlace")
	UAnimSequence* FallbackTurnInPlace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Category = "Settings|TurnInPlace", meta = (AllowInherited = true))
	TArray<UTurnInPlaceParams*> TurnInPlaceAnims;

	UFUNCTION(BlueprintCallable)
	void UpdateTurnInPlaceVelocityBuffer(float DeltaTime);

	float GetAvarageVelocity();

	UFUNCTION(BlueprintCallable)
	void UpdateTurnInPlace(float deltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTurnInPlaceState GetTurnInPlaceByAxis(EAxis::Type axis, FRotator deviation, float velocity);

	UFUNCTION(BlueprintCallable)
	void ApplyTurnInPlace();

	UPROPERTY(BlueprintReadWrite)
	bool IsTurning;

	UPROPERTY(BlueprintReadOnly)
	bool IsLockedByTurnInPlace;

	UFUNCTION()
	void UnlockTurnInPlace();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|TurnInPlace")
	float LockMarginTime;
	
	UPROPERTY(BlueprintReadOnly)
	FTurnInPlaceState CurrentTurningState = FTurnInPlaceState();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentTurningInPlaceWeight();

	UFUNCTION(BlueprintCallable)
	void SetIsTurning(bool turning);

	UPROPERTY(EditAnywhere)
	float VelocityBufferTimeInterval {0.25};

	UPROPERTY(EditAnywhere)
	int VelocityBufferMax { 4 };

private:

	UPROPERTY()
	FRotator InitialTurningDirection;

	UPROPERTY()
	FRotator TargetTurningDirection;

	UPROPERTY()
	float TurningProgression = 0;

	UPROPERTY()
	float CurrentTurningInPlaceWeight = 0;

	UPROPERTY()
	TArray<float> VelocityBuffer;

	UPROPERTY()
	float LastVelocityBufferTimeSpent;

};
