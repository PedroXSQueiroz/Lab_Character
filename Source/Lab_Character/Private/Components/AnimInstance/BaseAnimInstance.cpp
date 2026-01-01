// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimInstance/BaseAnimInstance.h"

#include "UObject/FastReferenceCollector.h"
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
            bool uncached = currentParams && !this->IKStatesCache.Contains(currentParams->Name);
            
            if (currentParams && currentParams->Enabled)
            {
                if (currentParams->WeightProvider)
                {
        
                    FIKState currentIKState = currentParams->GetIKState(this);

                    states.Add(currentIKState);

                    if (uncached) 
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
                if (uncached) 
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
        if (params && params->Enabled) 
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
    if (this->IsTurning || this->bUseDesiredForwardRotation) 
    {
        return FLeanStateBlendAnim(this->DefaultLeanAnim);
    }

    
    if (this->LeanByBlendParams.Contains(axis) 
        && this->LeanByBlendParams[axis] 
        && IsValid(this->LeanByBlendParams[axis])
        && this->LeanByBlendParams[axis]->Enabled)
    {
        return this->LeanByBlendParams[axis]->GetState(this, axis, this->MovementState);
    }
    
    return FLeanStateBlendAnim(this->DefaultLeanAnim);
}

void UBaseAnimInstance::UpdateTurnInPlaceVelocityBuffer(float DeltaTime)
{
    this->LastVelocityBufferTimeSpent += DeltaTime;

    if (this->LastVelocityBufferTimeSpent > this->VelocityBufferTimeInterval) 
    {
        this->VelocityBuffer.Add(this->MovementState.CurrentVelocity.Length());
    }

    if(this->VelocityBuffer.Num() > this->VelocityBufferMax)
    {
        this->VelocityBuffer.RemoveAt(0);
    }
}

float UBaseAnimInstance::GetAvarageVelocity()
{
    float sum = 0;

    for (float velocityEntry : this->VelocityBuffer) { sum += velocityEntry; }
    
    return sum / this->VelocityBuffer.Num();
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
    velocity = this->GetAvarageVelocity();

    switch (axis)
    {
    case EAxis::Type::X: deviationScalar = deviation.Roll; break;
    case EAxis::Type::Y: deviationScalar = deviation.Pitch; break;
    case EAxis::Type::Z: deviationScalar = deviation.Yaw; break;
    }

    if ((!this->IsTurning || this->CurrentTurningState.IsNone) && !this->IsLockedByTurnInPlace)
    {
        UTurnInPlaceParams* selectedTurnParams = NULL;

        for (UTurnInPlaceParams* turnParams : this->TurnInPlaceAnims) 
        {
            if (!turnParams || !IsValid(turnParams))
            {
                continue;
            }
            
            if (turnParams->DeviationAxis == axis && turnParams->Enabled) 
            {
                if ((   ( turnParams->MinDeviation >= 0 && deviationScalar > turnParams->MinDeviation )
                    ||  ( turnParams->MinDeviation <= 0 && deviationScalar < turnParams->MinDeviation )
                    )
                    && 
                    (
                        ( 
                            ( selectedTurnParams && turnParams->MinDeviation > selectedTurnParams->MinDeviation ) 
                        ||  ( selectedTurnParams && turnParams->MinDeviation < 0 && turnParams->MinDeviation < selectedTurnParams->MinDeviation)
                        ) 
                    ||  (!selectedTurnParams)
                    ) 
                    && (velocity >= turnParams->MinVelocity && ( velocity < turnParams->MaxVelocity || turnParams->MaxVelocity == -1 ) )
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
            this->MovementState.DirectionDeviation = FRotator::ZeroRotator;
            this->CurrentTurningState = FTurnInPlaceState(
                selectedTurnParams->TurnAnim, 
                0, 
                selectedTurnParams->TransitionOnFinish, 
                selectedTurnParams->DeviationAxis,
                selectedTurnParams->PlayRate,
                deviationScalar > 0
            );
            this->CurrentTurningState.Progression = this->GetCurrentTurningInPlaceWeight();
            this->InitialTurningDirection = this->GetOwningActor()->GetActorRotation();
            this->InitialTurningDirection.Normalize();
            this->TargetTurningDirection = this->MovementState.CurrentVelocity.IsZero() ? 
                                                this->DesiredForwardRotation 
                                            :   this->MovementState.CurrentVelocity.Rotation();
            this->TargetTurningDirection.Normalize();
            this->IsLockedByTurnInPlace = true;

            //WARNING: this should not be called here, but if it is called, is better keep consistence
            this->ClearUnlockTurnInPlaceTimer();

            UE_LOG(LogTemp, Log, TEXT("[TurnInPlace Debug] Playing turn: %s"), *selectedTurnParams->ParamName.ToString())

            return this->CurrentTurningState;
        }
        
        FTurnInPlaceState dummy( this->CurrentTurningState.TurnAnim ? this->CurrentTurningState.TurnAnim : this->FallbackTurnInPlace);
        dummy.IsNone = true;
        this->CurrentTurningState = dummy;
        return dummy;
    }
    
    return this->CurrentTurningState;
}

void UBaseAnimInstance::ApplyTurnInPlace()
{
    FRotator deviation = this->MovementState.DirectionDeviation;
    
    float deviationScalar = FMath::Max3(
        FMath::Abs(deviation.Roll), 
        FMath::Abs(deviation.Pitch),
        FMath::Abs(deviation.Yaw)
    );

    if (!FMath::IsNearlyZero( deviationScalar ) && (this->IsTurning && !this->CurrentTurningState.IsNone))
    { 
        this->CurrentTurningState.Progression = this->GetCurrentTurningInPlaceWeight();

        FRotator targetTurnDirection = this->MovementState.CurrentVelocity.Rotation();
        targetTurnDirection.Normalize();

        FRotator currentTargetDirection = this->MovementState.CurrentVelocity.IsZero() ?
            this->DesiredForwardRotation
            : this->MovementState.CurrentVelocity.Rotation();

        /*FRotator diffToTarget = UKismetMathLibrary::NormalizedDeltaRotator(this->InitialTurningDirection, this->TargetTurningDirection);

        FRotator currentTurnRot = FRotator( this->InitialTurningDirection );
        switch (this->CurrentTurningState.Axis)
        {
        case EAxis::Type::X : {
            float diff = diffToTarget.Roll;
            currentTurnRot.Roll += (diff * this->CurrentTurningState.Progression) * (diff < 0 ? 1 : -1);
        }
            break;
        case EAxis::Type::Y: {
            float diff = diffToTarget.Pitch;
            currentTurnRot.Pitch += (diff * this->CurrentTurningState.Progression) * (diff < 0 ? 1 : -1);
        }
            break;
        case EAxis::Type::Z: {
            float diff = diffToTarget.Yaw;
            currentTurnRot.Yaw += (diff * this->CurrentTurningState.Progression) * (diff < 0 ? 1 : 1);
        }
            break;
        }

        UE_LOG(LogTemp, Log, TEXT("Turn In Place diff to Target: %f, progression: %f"), diffToTarget.Yaw, this->CurrentTurningState.Progression)*/

        FRotator currentTurnRot = UKismetMathLibrary::RLerp(this->InitialTurningDirection, this->TargetTurningDirection, this->CurrentTurningState.Progression, true);

        this->GetOwningActor()->SetActorRotation(currentTurnRot);

        float clampVelocity = 0;
        this->GetCurveValue(FName("TurnClampVelocity"), clampVelocity);

        ABaseCharacters* charac = Cast<ABaseCharacters>(this->GetOwningActor());
        charac->SetClampVelocityInput(clampVelocity);

    }

}

void UBaseAnimInstance::UnlockTurnInPlace()
{
    this->IsLockedByTurnInPlace = false;
    //WARNING: this should not be called here, but if it is called, is better keep consistence
    this->ClearUnlockTurnInPlaceTimer();
    UE_LOG(LogTemp, Log, TEXT("[TurnInPlace Debug] Turn unlocked"))
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

            if (this->CurrentTurningState.TransitionOnFinishTurn) 
            {
                this->IsTransiting = true;
            }

            //WARNING: this should not be called here, but if it is called, is better keep consistence
            this->ClearUnlockTurnInPlaceTimer();

            GetWorld()
            ->GetTimerManager()
            .SetTimer(
                this->UnLockTurnInPlaceHandle,
                this,
                &UBaseAnimInstance::UnlockTurnInPlace,
                this->LockMarginTime,
                false
            );
        }
    }
}

void UBaseAnimInstance::ClearUnlockTurnInPlaceTimer()
{
    FTimerManager& currentTimeManager = this->GetWorld()->GetTimerManager();

    if (currentTimeManager.IsTimerActive(this->UnLockTurnInPlaceHandle)) 
    {
        currentTimeManager.ClearTimer(this->UnLockTurnInPlaceHandle);
    }
}

TArray<FLeanStateProcedural> UBaseAnimInstance::GetLeanProcStates()
{
    TArray<FLeanStateProcedural> states;
   
    this->ProcLeanStatesCache.GenerateValueArray(states);
    
    return states;
}

TArray<FLeanStateProcedural> UBaseAnimInstance::UpdateLeanProcStates()
{
    TArray<FLeanStateProcedural> states;

    for (ULeanParamProcedural* param : this->ProceduralLeans)
    {
        if (param)
        {
            param->CurrentWeight = !this->IsTurning && param->Enabled ?
                FMath::Lerp(param->CurrentWeight, 1, param->WeightLerp) :
                FMath::Lerp(param->CurrentWeight, 0, param->WeightLerp);

            UE_LOG(LogTemp, Log, TEXT("Weight for param: %s => %.2f"), *param->ParamName.ToString(), param->CurrentWeight);

            //TODO: THE CONDITIONAL COMMENTED BELOW WAS MADE FOR NOT CALCULATE WHE IS NOT NECCESSARY
            // BUT THIS APPROACH DESABLE A GRACFULL FALLOF OF THE LEAN
            // SHOULD BE POSSIBLE ENBALE THIS FALLOF AND YET NOT CALCULATE THIS VALUES WHE IS NOT NECCESSARY
            
            //if(param->Enabled)
            //{
                FLeanStateProcedural currentState = param->GetState(this, this->MovementState, param->CurrentWeight);
                
                if (this->ProcLeanStatesCache.Contains(param->ParamName))
                {
                    //INTERPOLE PREVIOUS TRANSFORMS WITH THE NEW ONE
                    FLeanStateProcedural previousState = this->ProcLeanStatesCache[param->ParamName];

                    for (int boneIndex = 0; boneIndex < previousState.TransformPerBone.Num(); boneIndex++)
                    {
                        currentState.TransformPerBone[boneIndex].Transform.Blend(
                            previousState.TransformPerBone[boneIndex].Transform,
                            currentState.TransformPerBone[boneIndex].Transform,
                            param->Lerp
                        );

                        /*currentState.TransformPerBone[boneIndex].Transform = FMath::Lerp(
                                previousState.TransformPerBone[boneIndex].Transform
                            ,   currentState.TransformPerBone[boneIndex].Transform
                            ,   param->Lerp
                        );*/
                    }

                    //STORE NEW STATE IN CACHE
                    this->ProcLeanStatesCache[param->ParamName] = currentState;
                }
                else
                {
                    this->ProcLeanStatesCache.Add(param->ParamName, currentState);
                }

                states.Add(currentState);
            } 
            //else if (this->ProcLeanStatesCache.Contains(param->ParamName))
            //{
            //    this->ProcLeanStatesCache.Remove(param->ParamName);
            //}
    }

    return states;
}

void UBaseAnimInstance::PostInitProperties()
{
    Super::PostInitProperties();

    TArray<UIKParams*>                                          iKParams            = this->DuplicateParams(this->IKParams);
    TArray<UIKRootParams*>                                      iKRootParams        = this->DuplicateParams(this->IKRootParams);
    TMap<TEnumAsByte<EAxis::Type>, ULeanParamBlendAnimByAxis*>  leanByBlendParams   = this->DuplicateParams(this->LeanByBlendParams);
    TArray<ULeanParamProcedural*>                               proceduralLeans     = this->DuplicateParams(this->ProceduralLeans);
    TArray<UTurnInPlaceParams*>                                 turnInPlaceAnims    = this->DuplicateParams(this->TurnInPlaceAnims);
    
    this->IKParams          = iKParams;
    this->IKRootParams      = iKRootParams;
    this->LeanByBlendParams = leanByBlendParams;
    this->ProceduralLeans   = proceduralLeans;
    this->TurnInPlaceAnims  = turnInPlaceAnims;

}

void UBaseAnimInstance::SetParamEnabled(FName paramName, bool enabled)
{
    this->SetParamEnabledOnList(this->IKParams, paramName, enabled );
    this->SetParamEnabledOnList(this->IKRootParams, paramName, enabled );

    TArray<ULeanParamBlendAnimByAxis*> leanParams;
    this->LeanByBlendParams.GenerateValueArray(leanParams);

    this->SetParamEnabledOnList(leanParams, paramName, enabled );
    
    this->SetParamEnabledOnList(this->ProceduralLeans, paramName, enabled );
    this->SetParamEnabledOnList(this->TurnInPlaceAnims, paramName, enabled);
}

void UBaseAnimInstance::ClearCaches()
{
    this->IKRootsStatesCache.Empty();
    this->IKStatesCache.Empty();
    this->ProcLeanStatesCache.Empty();
}

void UBaseAnimInstance::SetDesiredForwardRotation(FRotator rotation)
{
    this->DesiredForwardRotation = rotation;
    this->bUseDesiredForwardRotation = true;
}

void UBaseAnimInstance::DesableDesiredForwardRotation()
{
    this->DesiredForwardRotation = FRotator::ZeroRotator;
    this->bUseDesiredForwardRotation = false;
}

FRotator UBaseAnimInstance::GetCurrentDeviation()
{
    FRotator desiredDirection = this->bUseDesiredForwardRotation ?
                                    this->DesiredForwardRotation :
                                    GetOwningActor()->GetActorRotation();
                                    ;
    
    FVector currentDirection = this->GetOwningActor()->GetVelocity();

    if (currentDirection.Length() == 0) 
    {
        currentDirection = this->GetOwningActor()->GetActorForwardVector();
    }

    currentDirection.Normalize();
    FRotator currentDirectionRot = currentDirection.Rotation();

    return UKismetMathLibrary::RLerp(
        this->MovementState.DirectionDeviation,
        UKismetMathLibrary::NormalizedDeltaRotator(currentDirectionRot, desiredDirection),
        this->DeviationLerp,
        true
    );
}

template<typename Param>
bool UBaseAnimInstance::SetParamEnabledOnList(TArray<Param*> params, FName paramName, bool enabled)
{
    for (Param* param : params) 
    {
        if (param && param->ParamName == paramName) 
        {
            param->Enabled = enabled;
            return true;
        }
    }

    return false;
}

template<typename Param>
inline Param* UBaseAnimInstance::DuplicateParam(Param* param)
{
    return nullptr;
}

template<typename Param>
inline TArray<Param*> UBaseAnimInstance::DuplicateParams(TArray<Param*> param)
{
    TArray<Param*> aux;
    
    for (int index = 0; index < param.Num(); index++)
    {
        aux.Add(DuplicateObject(param[index], this));
    }

    return aux;

}

template<typename Param, typename Key>
inline TMap<Key, Param*> UBaseAnimInstance::DuplicateParams(TMap<Key, Param*> param)
{
    TMap<Key, Param*> aux;
    
    TArray<Key> keys;
    param.GenerateKeyArray(keys);

    for (Key key : keys)
    {
        aux.Add(key, DuplicateObject(param[key], this));
    }

    return aux;
}

template<typename Param>
void UBaseAnimInstance::CleanParams(TArray<Param*> param)
{
    for (Param* currentParam : param) 
    {
        currentParam->ConditionalBeginDestroy();
    }
}

template<typename Param, typename Key>
void UBaseAnimInstance::CleanParams(TMap<Key, Param*> param)
{
    TArray<Key> keys;
    param.GenerateKeyArray(keys);

    for (Key key : keys)
    {
        param[key]->ConditionalBeginDestroy();
    }
}

#pragma optimize("", on)
