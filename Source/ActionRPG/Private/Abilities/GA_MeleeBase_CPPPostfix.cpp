// Copyright Epic Games, Inc. All Rights Reserved.


#include "Abilities/GA_MeleeBase_CPPPostfix.h"
#include "Abilities/RPGAbilityTask_PlayMontageAndWaitForEvent.h"
#include "Abilities/GE_MeleeBase_CPPPostfix.h"



UGA_MeleeBase_CPPPostfix::UGA_MeleeBase_CPPPostfix()
{
	// AM_Attack �ִ� ��Ÿ�ָ� ����� ��Ÿ�ַ� ����
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageObj(TEXT("/Game/Characters/Animations/AM_Attack_Axe.AM_Attack_Axe"));

	if (AttackMontageObj.Succeeded())
	{
		MontageToPlay = AttackMontageObj.Object;
	}
	
	// TagContainer �� Tag �߰�
	EventTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Montage")));

	// Ŭ���� �⺻�� �±� �߰�
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Melee")));

	// ���ø����̼� ����
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	// �����÷��� ����Ʈ �ʿ� Ű�� ����� �߰�
	FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shared.WeaponHit"));
	FRPGGameplayEffectContainer RPGGEContainer;
	EffectContainerMap.Add(GameplayTag, RPGGEContainer);

}

// �����Ƽ ����
void UGA_MeleeBase_CPPPostfix::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// EndAbiliity ���� Ȱ���ϱ� ���� ���� ����
	CurHandle = Handle;
	CurActorinfo = *ActorInfo;
	CurActivationInfo = ActivationInfo;

	bool bCommitOk = CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (bCommitOk)
	{		
		// URPGAbilityTask_PlayMontageAndWaitForEvent �� �����Ѵ�. 
		WaitTask = URPGAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, TEXT(""), MontageToPlay, EventTags);
		
		// FScriptDelegate �� ������ ������ AbilityTask �� ��������Ʈ�� ȣ���ϰ��� �ϴ� �Լ��� Binding �Ѵ�.
		FScriptDelegate CancelledDelegate;
		CancelledDelegate.BindUFunction(this, FName("OnWaitTaskCancelled"));

		// ��Ÿ�ְ� ���� �ƿ��Ǵ� ���
		WaitTask->OnBlendOut.Add(CancelledDelegate);
		// �ٸ� ��Ÿ�ְ� ���� �� ���
		WaitTask->OnInterrupted.Add(CancelledDelegate);
		// �۾��� ��ҵǴ� ���
		WaitTask->OnCancelled.Add(CancelledDelegate);


		FScriptDelegate TaskFinishedDelegate;
		TaskFinishedDelegate.BindUFunction(this, FName("OnWaitTaskFinished"));
		// EventTags�� ��ġ�ϴ� �����÷��� �̺�Ʈ�� �߻��� ���
		WaitTask->EventReceived.Add(TaskFinishedDelegate);


		WaitTask->Activate();
	}
}

// PlayMontageAndWaitForEvent �� ����� ������ ����Ǿ��� ��� ����
void UGA_MeleeBase_CPPPostfix::OnWaitTaskCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{	
	UGameplayAbility::EndAbility(CurHandle, &CurActorinfo, CurActivationInfo, true, false);
}

// EventTags�� ��ġ�ϴ� �����÷��� �̺�Ʈ�� �߻��� ��� ����
void UGA_MeleeBase_CPPPostfix::OnWaitTaskFinished(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ApplyEffectContainer(EventTag, EventData, -1);
}
