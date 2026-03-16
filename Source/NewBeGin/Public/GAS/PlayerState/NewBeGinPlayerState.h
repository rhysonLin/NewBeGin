#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "NewBeGinPlayerState.generated.h"

class UAbilitySystemComponent;
class UNewBeGinAbilitySystemComponent;
class UNewBeGinAttributeSet;

/**
 * PlayerState 持有 ASC + AttributeSet
 * 这是 GAS 推荐架构
 */
UCLASS()
class NEWBEGIN_API ANewBeGinPlayerState
	: public APlayerState
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ANewBeGinPlayerState();

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** 获取自定义 ASC */
	UNewBeGinAbilitySystemComponent* GetASC() const;

	/** 获取属性集 */
	UNewBeGinAttributeSet* GetAttributeSet() const;

protected:
	/** GAS 核心组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UNewBeGinAbilitySystemComponent> AbilitySystemComponent;

	/** 属性集：血量等 */
	UPROPERTY()
	TObjectPtr<UNewBeGinAttributeSet> AttributeSet;
};