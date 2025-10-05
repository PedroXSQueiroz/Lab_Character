// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/States/AimingState.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Entities/Characters/BaseCharacters.h"

bool UAimingState::CharacterInit(ABaseCharacters* charac)
{
	
	
	if (Super::CharacterInit(charac)) 
	{
		//charac->GetBinding(FName("aim"))->InputBindingTrigger.AddDynamic(this, &UAimingState::StopAiming);
		charac->bUseControllerRotationYaw = false;
		charac->GetCharacterMovement()->bOrientRotationToMovement = false;

		UAnimInstance* animInstance = charac->GetMesh()->GetAnimInstance();
		this->CachedAnimInstance = Cast<UBaseAnimInstance>(animInstance);

		TArray<AActor*> targets;

		UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), FName("target"), targets);

		if (!targets.IsEmpty()) 
		{
			this->CurrentTarget = targets[0];
		}
	

		return true;
	}

	return false;
}

void UAimingState::StatefullTick(AActor* actor, float DeltaTime)
{
	Super::StatefullTick(actor, DeltaTime);

	ACharacter* charac = Cast<ACharacter>(actor);

	if (this->CurrentTarget) 
	{
		FVector directionToTarget = UKismetMathLibrary::GetDirectionUnitVector(
			this->GetOwner()->GetActorLocation(),
			this->CurrentTarget->GetActorLocation()
		);
	
		FRotator forwardToTargetRot = directionToTarget.Rotation();
		forwardToTargetRot.Pitch = 0;
		forwardToTargetRot.Roll = 0;

		this->CachedAnimInstance->SetDesiredForwardRotation(forwardToTargetRot);
		actor->SetActorRotation(forwardToTargetRot);
	}

	//this->CachedAnimInstance->SetDesiredForwardRotation(charac->GetControlRotation());
}

void UAimingState::StopAiming(const FInputActionValue& aim)
{
	this->CachedAnimInstance->SetDesiredForwardRotation(FRotator::ZeroRotator);
	this->ChangeTo(FName("default"));
}
