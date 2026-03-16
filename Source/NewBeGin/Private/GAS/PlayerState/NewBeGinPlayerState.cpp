#include "GAS/PlayerState/NewBeGinPlayerState.h"

#include "GAS/ASC/NewBeGinAbilitySystemComponent.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"
#include "AbilitySystemComponent.h"

ANewBeGinPlayerState::ANewBeGinPlayerState()
{
	// 创建 ASC
	AbilitySystemComponent =
		CreateDefaultSubobject<UNewBeGinAbilitySystemComponent>(
			TEXT("AbilitySystemComponent"));

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(
		EGameplayEffectReplicationMode::Mixed);

	// 创建 AttributeSet
	AttributeSet =
		CreateDefaultSubobject<UNewBeGinAttributeSet>(
			TEXT("AttributeSet"));
}

UAbilitySystemComponent* ANewBeGinPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UNewBeGinAbilitySystemComponent* ANewBeGinPlayerState::GetASC() const
{
	return AbilitySystemComponent;
}

UNewBeGinAttributeSet* ANewBeGinPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}