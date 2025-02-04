// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimInstance/BaseAnimInstance.h"

#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Character.h>

#include <Entities/Characters/BaseCharacters.h>
#include <Data/WeightProvider.h>

#pragma optimize("", off)
void UBaseAnimInstance::InitIKParams()
{
    for (UIKParams* param : this->IKParams) 
    {
        param->WeightProvider->Init(this);
    }
}

void UBaseAnimInstance::UpdateMovementState()
{
    AActor* owner = this->GetOwningActor();
    
    this->MovementState.DirectionDeviation = this->GetCurrentDeviation();
    this->MovementState.CurrentVelocity = owner->GetVelocity();
    this->MovementState.VelocityScale = this->MovementState.CurrentVelocity.Length() / this->MaxVelocity;
}

TArray<FIKState> UBaseAnimInstance::UpdateCurrentIKsStates()
{
    TArray<FIKState> states = TArray<FIKState>();
    try 
    {
        for (UIKParams* currentParams : this->IKParams)
        {
            bool cached = !this->IKStatesCache.Contains(currentParams->Name);
            
            if (currentParams && currentParams->Enabled)
            {
                if (currentParams->WeightProvider)
                {
        
                    FIKState currentIKState = currentParams->GetIKState(this);

                    states.Add(currentIKState);

                    if (cached) 
                    {
                        this->IKStatesCache.Add(currentParams->Name, currentIKState);
                    }
                    else 
                    {
                        this->IKStatesCache[currentParams->Name] = currentIKState;
                    }
                }
            }
            else 
            {
                if (cached) 
                {
                    this->IKStatesCache.Remove(currentParams->Name);
                }
            }
        }
    
    }
    catch (const std::exception&)
    {
        UE_LOG(LogTemp, Error, TEXT("Error on calculate IKs"));
    }
    
    return states;
}

TArray<FIKState> UBaseAnimInstance::GetCurrentIKStates()
{
    TArray<FIKState> ikStates;
    
    this->IKStatesCache.GenerateValueArray(ikStates);
    
    return ikStates;
}

FIKState UBaseAnimInstance::GetCurrentIKStateByName(FName name, bool& found)
{
    found = this->IKStatesCache.Contains(name);
    if (found)
    {
        return this->IKStatesCache[name];
    }

    return FIKState();
}

TArray<FIKRootState> UBaseAnimInstance::UpdateCurrentIKRootStates()
{
    TArray<FIKRootState> states;

    for (UIKRootParams* params : this->IKRootParams) 
    {
        if (params->Enabled) 
        {
            FIKRootState currentState = params->GetCurrentRootTransform(this);

            states.Add(currentState);
            this->IKRootsStatesCache.Add(params->Name, currentState);
        }
        
    }

    return states;
}

TArray<FIKRootState> UBaseAnimInstance::GetCurrentRootIKStates()
{
    TArray<FIKRootState> states;
    
    this->IKRootsStatesCache.GenerateValueArray(states);
    
    return states;
}

FLeanStateBlendAnim UBaseAnimInstance::GetLeanByBlendAnimByAxis(EAxis::Type axis)
{
    if (this->LeanByBlendParams.Contains(axis) && this->LeanByBlendParams[axis] && this->LeanByBlendParams[axis]->Enabled)
    {
        return this->LeanByBlendParams[axis]->GetState(this, axis, this->MovementState);
    }
    
    return FLeanStateBlendAnim(this->DefaultLeanAnim);
}

void UBaseAnimInstance::UpdateTurnInPlace(float deltaTime)
{
    this->GetTurnInPlaceByAxis(EAxis::Type::X, this->MovementState.DirectionDeviation, this->MovementState.VelocityScale);
    this->GetTurnInPlaceByAxis(EAxis::Type::Y, this->MovementState.DirectionDeviation, this->MovementState.VelocityScale);
    this->GetTurnInPlaceByAxis(EAxis::Type::Z, this->MovementState.DirectionDeviation, this->MovementState.VelocityScale);

    if (this->IsTurning) 
    {
        this->TurningProgression += deltaTime;
    }
}

FTurnInPlaceState UBaseAnimInstance::GetTurnInPlaceByAxis(EAxis::Type axis, FRotator deviation, float velocity)
{
    float deviationScalar = 0;

    switch (axis)
    {
    case EAxis::Type::X: deviationScalar = deviation.Roll; break;
    case EAxis::Type::Y: deviationScalar = deviation.Pitch; break;
    case EAxis::Type::Z: deviationScalar = deviation.Yaw; break;
    }

    if (!this->IsTurning || this->CurrentTurningState.IsNone) 
    {
        UTurnInPlaceParams* selectedTurnParams = NULL;

        for (UTurnInPlaceParams* turnParams : this->TurnInPlaceAnims) 
        {
            if (!turnParams) 
            {
                continue;
            }
            
            if (turnParams->DeviationAxis == axis) 
            {
                UE_LOG(LogTemp, Log, TEXT("current deviation: %.2f"), deviationScalar);

                if ((   ( turnParams->MinDeviation > 0 && deviationScalar > turnParams->MinDeviation )
                    ||  ( turnParams->MinDeviation < 0 && deviationScalar < turnParams->MinDeviation )
                    )
                    && 
                    (
                        ( 
                            ( selectedTurnParams && turnParams->MinDeviation > selectedTurnParams->MinDeviation ) 
                        ||  ( selectedTurnParams && turnParams->MinDeviation < 0 && turnParams->MinDeviation < selectedTurnParams->MinDeviation)
                        ) 
                    ||  (!selectedTurnParams)
                    ) 
                    && (velocity > turnParams->MinVelocity)
                    && 
                    (
                        ( selectedTurnParams && turnParams->MinVelocity > selectedTurnParams->MinVelocity )
                    ||  (!selectedTurnParams)
                    )
                ) 
                {
                    selectedTurnParams = turnParams;
                }
            }
        }

        if (selectedTurnParams) 
        {
            this->SetIsTurning(true);
            this->TurningProgression = 0;
            this->CurrentTurningState = FTurnInPlaceState(selectedTurnParams->TurnAnim);
            this->CurrentTurningState.Progression = this->GetCurrentTurningInPlaceWeight();
            this->InitialTurningDirection = this->GetOwningActor()->GetActorRotation();
            this->InitialTurningDirection.Normalize();
            this->TargetTurningDirection = this->MovementState.CurrentVelocity.Rotation();
            this->TargetTurningDirection.Normalize();

            return this->CurrentTurningState;
        }
        
        FTurnInPlaceState dummy( this->CurrentTurningState.TurnAnim ? this->CurrentTurningState.TurnAnim : this->FallbackTurnInPlace);
        dummy.IsNone = true;
        this->CurrentTurningState = dummy;
        return dummy;
    }
    else if(deviationScalar != 0)
    {
        this->CurrentTurningState.Progression = this->GetCurrentTurningInPlaceWeight();

        FRotator targetTurnDirection = this->MovementState.CurrentVelocity.Rotation();
        targetTurnDirection.Normalize();
        
        FRotator currentTurnRot = UKismetMathLibrary::RLerp(this->InitialTurningDirection, this->TargetTurningDirection, this->CurrentTurningState.Progression, true);

        this->GetOwningActor()->SetActorRotation(currentTurnRot);

        ABaseCharacters* charac = Cast<ABaseCharacters>(this->GetOwningActor());
        charac->SetClampVelocityInput(1 - this->CurrentTurningState.Progression);

    }
    
    UE_LOG(LogTemp, Log, TEXT("current deviation: %.2f"), deviationScalar);

    return this->CurrentTurningState;
}

float UBaseAnimInstance::GetCurrentTurningInPlaceWeight()
{
    if (!this->CurrentTurningState.IsNone) 
    {
        TArray<FFloatCurve> turnInPlaceWeightCurve = this->CurrentTurningState.TurnAnim->GetCurveData().FloatCurves.FilterByPredicate([](const FFloatCurve& curve) {
            return curve.GetName().IsEqual(FName("TurnInPlaceWeitgh"));
        });

        if (turnInPlaceWeightCurve.Num() == 1)
        {
            return turnInPlaceWeightCurve[0].FloatCurve.Eval(this->TurningProgression);
        }
    }
    
    return 0.0f;
}

void UBaseAnimInstance::SetIsTurning(bool turning)
{
    UE_LOG(LogTemp, Log, TEXT("UPDATE URNING STATE: %s"), turning? *FString("YES") : *FString("NO"))
    
    this->IsTurning = turning;
    ABaseCharacters* charac = Cast<ABaseCharacters>(this->GetOwningActor());
    if (charac) 
    {
        charac->GetCharacterMovement()->bOrientRotationToMovement = !turning;

        if (!turning) 
        {
            charac->SetClampVelocityInput(0);
        }
    }
}

TArray<FLeanStateProcedural> UBaseAnimInstance::GetLeanProcStates()
{
    TArray<FLeanStateProcedural> states;
    
    for (ULeanParamProcedural* param : this->ProceduralLeans) 
    {
        if (param->Enabled) 
        {
            states.Add(param->GetState(this, this->MovementState));
        }
    }
    
    return states;
}

void UBaseAnimInstance::ClearCaches()
{
    this->IKRootsStatesCache.Empty();
    this->IKStatesCache.Empty();
}

void UBaseAnimInstance::SetDesiredForwardRotation(FRotator rotation)
{
    this->DesiredForwardRotation = rotation;
}

FRotator UBaseAnimInstance::GetCurrentDeviation()
{
    FRotator desiredDirection = this->DesiredForwardRotation.IsZero() ? 
                                GetOwningActor()->GetActorRotation() : 
                                this->DesiredForwardRotation;
    FVector currentVelocity = this->GetOwningActor()->GetVelocity();

    if (currentVelocity.Length() == 0) 
    {
        return FRotator::ZeroRotator;
    }

    currentVelocity.Normalize();
    FRotator currentDirection = currentVelocity.Rotation();

    return UKismetMathLibrary::NormalizedDeltaRotator(currentDirection, desiredDirection);
}
#pragma optimize("", on)
