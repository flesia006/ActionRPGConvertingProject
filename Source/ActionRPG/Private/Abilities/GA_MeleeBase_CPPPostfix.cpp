// Copyright Epic Games, Inc. All Rights Reserved.


#include "Abilities/GA_MeleeBase_CPPPostfix.h"
#include "Abilities/RPGAbilityTask_PlayMontageAndWaitForEvent.h"
#include "Abilities/GE_MeleeBase_CPPPostfix.h"



UGA_MeleeBase_CPPPostfix::UGA_MeleeBase_CPPPostfix()
{
	// AM_Attack 애님 몽타주를 재생할 몽타주로 설정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageObj(TEXT("/Game/Characters/Animations/AM_Attack_Axe.AM_Attack_Axe"));

	if (AttackMontageObj.Succeeded())
	{
		MontageToPlay = AttackMontageObj.Object;
	}
	
	// TagContainer 에 Tag 추가
	EventTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Montage")));

	// 클래스 기본값 태그 추가
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Melee")));

	// 리플리케이션 설정
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	// 게임플레이 이펙트 맵에 키와 밸류를 추가
	FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shared.WeaponHit"));
	FRPGGameplayEffectContainer RPGGEContainer;
	EffectContainerMap.Add(GameplayTag, RPGGEContainer);

}

// 어빌리티 시작
void UGA_MeleeBase_CPPPostfix::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// EndAbiliity 에서 활용하기 위한 변수 정의
	CurHandle = Handle;
	CurActorinfo = *ActorInfo;
	CurActivationInfo = ActivationInfo;

	bool bCommitOk = CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (bCommitOk)
	{		
		// URPGAbilityTask_PlayMontageAndWaitForEvent 를 생성한다. 
		WaitTask = URPGAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, TEXT(""), MontageToPlay, EventTags);
		
		// FScriptDelegate 로 선언한 변수를 AbilityTask 의 델리게이트로 호출하고자 하는 함수에 Binding 한다.
		FScriptDelegate CancelledDelegate;
		CancelledDelegate.BindUFunction(this, FName("OnWaitTaskCancelled"));

		// 몽타주가 블렌드 아웃되는 경우
		WaitTask->OnBlendOut.Add(CancelledDelegate);
		// 다른 몽타주가 덮어 쓴 경우
		WaitTask->OnInterrupted.Add(CancelledDelegate);
		// 작업이 취소되는 경우
		WaitTask->OnCancelled.Add(CancelledDelegate);


		FScriptDelegate TaskFinishedDelegate;
		TaskFinishedDelegate.BindUFunction(this, FName("OnWaitTaskFinished"));
		// EventTags와 일치하는 게임플레이 이벤트가 발생한 경우
		WaitTask->EventReceived.Add(TaskFinishedDelegate);


		WaitTask->Activate();
	}
}

// PlayMontageAndWaitForEvent 가 대기중 비정상 종료되었을 경우 실행
void UGA_MeleeBase_CPPPostfix::OnWaitTaskCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{	
	UGameplayAbility::EndAbility(CurHandle, &CurActorinfo, CurActivationInfo, true, false);
}

// EventTags와 일치하는 게임플레이 이벤트가 발생한 경우 실행
void UGA_MeleeBase_CPPPostfix::OnWaitTaskFinished(FGameplayTag EventTag, FGameplayEventData EventData)
{
	ApplyEffectContainer(EventTag, EventData, -1);
}
