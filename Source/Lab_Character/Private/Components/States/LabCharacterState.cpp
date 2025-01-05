// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/States/LabCharacterState.h"

#include "Entities/Characters/BaseCharacters.h"
#include <Components/AnimInstance/BaseAnimInstance.h>
#include "Data/LabAnimParams.h"


bool ULabCharacterState::Init(AActor* actor, bool first)
{
	ABaseCharacters* charac = Cast<ABaseCharacters>(actor);

	UAnimInstance* animInstance = charac->GetMesh()->GetAnimInstance();
	UBaseAnimInstance* labAnim = Cast<UBaseAnimInstance>(animInstance);

	this->UpdateAnimParams(labAnim->IKParams);
	this->UpdateAnimParams(labAnim->IKRootParams);
	
	TArray<ULeanParamBlendAnimByAxis*> leanParamsByBlendAnim;
	labAnim->LeanByBlendParams.GenerateValueArray(leanParamsByBlendAnim);
	this->UpdateAnimParams(leanParamsByBlendAnim);

	this->UpdateAnimParams(labAnim->ProceduralLeans);

	if (this->CharacterInit(charac))
	{
		return Super::Init(actor, first);
	}

	return false;
}

bool ULabCharacterState::Exit(AActor* actor)
{
	ABaseCharacters* charac = Cast<ABaseCharacters>(actor);

	if (charac) 
	{
		charac->CleanBindings();
	
		return this->CharacterExit(charac);
	}
	
	return false;
}

bool ULabCharacterState::CharacterExit(ABaseCharacters* charac)
{
	return true;
}

bool ULabCharacterState::CharacterInit(ABaseCharacters* charac)
{
	return true;
}

ABaseCharacters* ULabCharacterState::GetCharacter()
{
	return Cast<ABaseCharacters>(this->GetOwner());
}

template<typename AnimParam>
void ULabCharacterState::UpdateAnimParams(TArray<AnimParam*> params)
{
	for (ULabAnimParams* param : params) 
	{
		bool foundParam = this->ActiveAnimParams.Contains(param->ParamName);
		param->Enabled = foundParam ? true : false;
	}
}
