// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/IKParams.h"

#include "Data/WeightProvider.h"

#include "Components/AnimInstance/BaseAnimInstance.h"

#include <Kismet/KismetMathLibrary.h>

#pragma optimize("", off)
FTransform UIKParamsByTrace::GetEffectorTransform(
        UAnimInstance* anim
    ,   FTransform previewsTransform
    ,   float weight
    ,   float& distance
    ,   FRotator& effectorRotation
)
{
    FVector reverseMask = FVector(1) - this->TraceDirection;
    
    FVector boneLocation = anim->GetSkelMeshComponent()->GetSocketLocation(this->Reference);
    FVector startTraceLocation =    ( boneLocation * reverseMask ) 
                                +   ( anim->GetSkelMeshComponent()->GetComponentLocation() * this->TraceDirection) 
                                +   ( this->TraceDirection * this->StartPointDistance);
    FVector endTraceLocation = startTraceLocation + (this->TraceDirection * this->EndPointDistance);

    if (this->DebugTraces) 
    {
        DrawDebugSphere(
            anim->GetWorld(),
            startTraceLocation,
            this->SweepRadius,
            12,
            FColor::Blue
        );

        DrawDebugSphere(
            anim->GetWorld(),
            endTraceLocation,
            this->SweepRadius,
            12,
            FColor::Blue
        );

        DrawDebugLine(
            anim->GetWorld(),
            startTraceLocation,
            endTraceLocation,
            FColor::Blue
        );
    }
    
    FHitResult hittedData;
    bool hitted = anim->GetWorld()->SweepSingleByChannel(
        hittedData,
        startTraceLocation,
        endTraceLocation,
        FQuat::Identity,
        ECollisionChannel::ECC_Visibility,
        FCollisionShape::MakeSphere(this->SweepRadius)
    );

    if (hitted) 
    {
        float deviation = 1 - this->TraceDirection.Dot(hittedData.Normal);
        
        if (deviation > this->MaxNormalDeviation) 
        {
            return FTransform(FQuat::Identity, boneLocation);
        }
        
        
        UE_LOG(LogTemp, Log, TEXT("[%s] original boneLocation: %s"), *this->ParamName.ToString(), *boneLocation.ToString());

        FVector newLocation             = hittedData.ImpactPoint;
        FVector dealocation             = ((boneLocation - anim->GetOwningComponent()->GetComponentLocation()) * this->TraceDirection);
        FVector nonLockedDealocation    = newLocation + dealocation;
        FVector lockedDealocation       = ( nonLockedDealocation * this->TraceDirection) + ( previewsTransform.GetLocation() * reverseMask );
        FVector finalLocation           = FMath::Lerp(nonLockedDealocation, lockedDealocation, weight);
        distance = FVector::Distance(startTraceLocation, hittedData.ImpactPoint);

        if (this->DebugTraces) 
        {
            DrawDebugSphere(
                anim->GetWorld(),
                newLocation,
                this->SweepRadius,
                12,
                FColor::Green
            );

            DrawDebugSphere(
                anim->GetWorld(),
                finalLocation,
                this->SweepRadius,
                12,
                FColor(weight * 255, 0, 255 - (weight * 255))
            );
        }

        if (this->RotationCalc) 
        {
            FRotator newRotation = this->RotationCalc->GetRotationFromTraceHit(hittedData, anim, this);
            effectorRotation.Pitch = newRotation.Pitch;
            effectorRotation.Yaw = newRotation.Yaw;
            effectorRotation.Roll = newRotation.Roll;
        }

        return FTransform(
            FQuat::Identity,
            finalLocation
        );
    }
	
	return FTransform(
        FQuat::Identity,
        boneLocation
    );
}
#pragma optimize("", on)

FIKState UIKParams::GetIKState(UBaseAnimInstance* anim)
{
    bool existsPreviews;
    
    FIKState previewsState = anim->GetCurrentIKStateByName(this->Name, existsPreviews);
    
    FTransform previewsIKTransform = existsPreviews ?
        previewsState.EffectorTransform :
        FTransform();

    float distance = 0;
    FRotator effectorRotator = FRotator::ZeroRotator;
    FTransform currentIKTransform = this->GetEffectorTransform(
            anim
        ,   previewsIKTransform
        ,   !existsPreviews ? 0 : this->WeightProvider->GetCurrentWeight()
        ,   distance
        ,   effectorRotator
    );

    FTransform bodyTransform = FTransform(
        anim->GetOwningComponent()->GetComponentQuat(),
        anim->GetOwningComponent()->GetComponentLocation()
    );

    FIKState currentIKState = FIKState(
        this->Name,
        this->Root,
        this->Effector,
        currentIKTransform,
        FTransform(
            effectorRotator,
            bodyTransform.InverseTransformPosition(currentIKTransform.GetLocation())
        ), this->MinAnglePivot,
        this->MaxAnglePivot,
        this->PivotBoneName,
        distance
    );
    
    return currentIKState;
}

FRotator UIKCustomRotationHorizontal::GetRotationFromTraceHit(FHitResult hit, UAnimInstance* anim, UIKParamsByTrace* traceParams)
{
    float asideAlignment = UKismetMathLibrary::DegAtan2(hit.Normal.Y, hit.Normal.Z);

    float forwardAlignment = UKismetMathLibrary::DegAtan2(hit.Normal.X, hit.Normal.Z) * -1;

    return FRotator(
        forwardAlignment,
        0,
        asideAlignment
    );
}
