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
 * - 自己持有 ASC
 * - 自己持有 AttributeSet
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

	/** 是否已经死亡，避免重复执行死亡逻辑 */
	UPROPERTY(VisibleAnywhere, Category="Death")
	bool bIsDead = false;
	

public:
	UNewBeGinAbilitySystemComponent* GetASC() const { return AbilitySystemComponent; }
	UNewBeGinAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/** 敌人死亡入口 */
	UFUNCTION(BlueprintCallable, Category="Death")
	void Die();
};