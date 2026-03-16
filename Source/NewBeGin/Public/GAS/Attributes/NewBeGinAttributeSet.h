#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NewBeGinAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * GAS 属性集
 * 放角色数值：血量、最大血量、伤害等
 */
UCLASS()
class NEWBEGIN_API UNewBeGinAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UNewBeGinAttributeSet();

	// =========================
	// 血量
	// =========================

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category="Attributes|Health")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category="Attributes|Health")
	FGameplayAttributeData MaxHealth;

	// =========================
	// 临时伤害属性
	// GameplayEffect 会把伤害先写到这里，
	// 然后在 PostGameplayEffectExecute 中转成 Health 扣减
	// =========================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category="Attributes|Damage")
	FGameplayAttributeData Damage;

	ATTRIBUTE_ACCESSORS(UNewBeGinAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UNewBeGinAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UNewBeGinAttributeSet, Damage)

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};