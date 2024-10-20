// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GA_AbilityBase_CPPPostfix.h"
#include "GA_MeleeBase_CPPPostfix.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UGA_MeleeBase_CPPPostfix : public UGA_AbilityBase_CPPPostfix
{
	GENERATED_BODY()

	UGA_MeleeBase_CPPPostfix();
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnWaitTaskFinished(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnWaitTaskCancelled(FGameplayTag EventTag, FGameplayEventData EventData);


private:
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	class URPGAbilityTask_PlayMontageAndWaitForEvent* WaitTask;

	FGameplayAbilitySpecHandle CurHandle;
	FGameplayAbilityActorInfo CurActorinfo;
	FGameplayAbilityActivationInfo CurActivationInfo;

};
