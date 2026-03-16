#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NewBeGinAbilitySystemComponent.generated.h"

/**
 * 项目自定义 AbilitySystemComponent
 * 先做最小封装：避免直接在别处调用 Super 的 InitAbilityActorInfo
 */
UCLASS()
class NEWBEGIN_API UNewBeGinAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	 * 初始化 GAS 的 Owner / Avatar 关联
	 * @param InOwnerActor  通常是 PlayerState（最正规结构里）
	 * @param InAvatarActor 通常是 Character（玩家当前控制的 Pawn）
	 */
	void NB_InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
};