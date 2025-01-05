// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimInstance/BaseAnimInstance.h"

#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Character.h>

#include <Data/WeightProvider.h>

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

#pragma optimize("", off)
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
    if (this->LeanByBlendParams.Contains(axis) && this->LeanByBlendParams[axis]->Enabled)
    {
        return this->LeanByBlendParams[axis]->GetState(this, axis, this->MovementState);
    }
    
    return FLeanStateBlendAnim(this->DefaultLeanAnim);
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
