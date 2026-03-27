#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "NewBeGinHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;
class UNewBeGinAttributeSet;

/**
 * 玩家 HUD Widget
 * 事件驱动刷新血量 UI
 */
UCLASS()
class NEWBEGIN_API UNewBeGinHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	/** 绑定到蓝图里的血条 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	/** 绑定到蓝图里的血量文本 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	/** 刷新 UI */
	UFUNCTION(BlueprintCallable, Category="HUD")
	void RefreshHealth();

	/** 获取玩家 AttributeSet */
	UNewBeGinAttributeSet* GetPlayerAttributeSet() const;

	/** 获取玩家 ASC */
	UAbilitySystemComponent* GetPlayerASC() const;

	/** 绑定属性变化委托 */
	void BindAttributeDelegates();

	/** Health 变化回调 */
	void OnHealthChanged(const FOnAttributeChangeData& ChangeData);

	/** MaxHealth 变化回调 */
	void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
};