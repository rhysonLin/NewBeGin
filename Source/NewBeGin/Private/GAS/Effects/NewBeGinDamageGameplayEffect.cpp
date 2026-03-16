#include "GAS/Effects/NewBeGinDamageGameplayEffect.h"

#include "GameplayEffect.h"
#include "GameplayTagsManager.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"

UNewBeGinDamageGameplayEffect::UNewBeGinDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UNewBeGinAttributeSet::GetDamageAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;

	// 通过 SetByCaller 动态传入伤害值
	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));

	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(Modifier);
}