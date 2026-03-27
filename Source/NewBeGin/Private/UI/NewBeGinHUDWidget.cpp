#include "UI/NewBeGinHUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"

#include "GAS/PlayerState/NewBeGinPlayerState.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"

void UNewBeGinHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 先刷新一次初始值
	RefreshHealth();

	// 再绑定事件，让之后的变化自动刷新
	BindAttributeDelegates();
}

UNewBeGinAttributeSet* UNewBeGinHUDWidget::GetPlayerAttributeSet() const
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ANewBeGinPlayerState* PS = PC->GetPlayerState<ANewBeGinPlayerState>();
	if (!PS) return nullptr;

	return PS->GetAttributeSet();
}

UAbilitySystemComponent* UNewBeGinHUDWidget::GetPlayerASC() const
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	ANewBeGinPlayerState* PS = PC->GetPlayerState<ANewBeGinPlayerState>();
	if (!PS) return nullptr;

	return PS->GetAbilitySystemComponent();
}

void UNewBeGinHUDWidget::RefreshHealth()
{
	UNewBeGinAttributeSet* AS = GetPlayerAttributeSet();
	if (!AS) return;

	const float Health = AS->GetHealth();
	const float MaxHealth = AS->GetMaxHealth();
	const float Percent = MaxHealth > 0.f ? Health / MaxHealth : 0.f;

	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}

	if (HealthText)
	{
		HealthText->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)
		));
	}
}

void UNewBeGinHUDWidget::BindAttributeDelegates()
{
	UAbilitySystemComponent* ASC = GetPlayerASC();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HUD] BindAttributeDelegates failed: ASC is null"));
		return;
	}

	UNewBeGinAttributeSet* AS = GetPlayerAttributeSet();
	if (!AS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HUD] BindAttributeDelegates failed: AttributeSet is null"));
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddUObject(
		this,
		&UNewBeGinHUDWidget::OnHealthChanged
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddUObject(
		this,
		&UNewBeGinHUDWidget::OnMaxHealthChanged
	);

	UE_LOG(LogTemp, Warning, TEXT("[HUD] Attribute delegates bound"));
}

void UNewBeGinHUDWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("[HUD] Health changed: %.1f -> %.1f"),
		ChangeData.OldValue,
		ChangeData.NewValue);

	RefreshHealth();
}

void UNewBeGinHUDWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("[HUD] MaxHealth changed: %.1f -> %.1f"),
		ChangeData.OldValue,
		ChangeData.NewValue);

	RefreshHealth();
}