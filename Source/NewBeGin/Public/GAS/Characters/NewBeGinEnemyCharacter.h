#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "NewBeGinEnemyCharacter.generated.h"

class UAbilitySystemComponent;
class UNewBeGinAbilitySystemComponent;
class UNewBeGinAttributeSet;

/**
 * GAS 敌人角色
 * 
 * 和玩家不同：
 * - 玩家：ASC 放 PlayerState（最正规）
 * - 敌人：ASC 直接放在 Character 上（最常见、最合理）
 */
UCLASS()
class NEWBEGIN_API ANewBeGinEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ANewBeGinEnemyCharacter();

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	/** GAS 核心组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UNewBeGinAbilitySystemComponent> AbilitySystemComponent;

	/** 属性集 */
	UPROPERTY()
	TObjectPtr<UNewBeGinAttributeSet> AttributeSet;

public:
	UNewBeGinAbilitySystemComponent* GetASC() const { return AbilitySystemComponent; }
	UNewBeGinAttributeSet* GetAttributeSet() const { return AttributeSet; }
};