#include "GAS/Characters/NewBeGinEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GAS/ASC/NewBeGinAbilitySystemComponent.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"

ANewBeGinEnemyCharacter::ANewBeGinEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 创建 ASC
	AbilitySystemComponent =
		CreateDefaultSubobject<UNewBeGinAbilitySystemComponent>(
			TEXT("AbilitySystemComponent"));

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// 创建 AttributeSet
	AttributeSet =
		CreateDefaultSubobject<UNewBeGinAttributeSet>(
			TEXT("AttributeSet"));
}

void ANewBeGinEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// 对敌人来说：
		// Owner = 自己
		// Avatar = 自己
		AbilitySystemComponent->NB_InitAbilityActorInfo(this, this);
	}

	if (AttributeSet && AbilitySystemComponent)
	{
		AbilitySystemComponent->SetNumericAttributeBase(AttributeSet->GetMaxHealthAttribute(), 100.f);
		AbilitySystemComponent->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), 100.f);

		UE_LOG(LogTemp, Warning, TEXT("[EnemyGAS] Init OK: %s Health=%.1f MaxHealth=%.1f"),
			*GetName(),
			AttributeSet->GetHealth(),
			AttributeSet->GetMaxHealth());
	}
}

UAbilitySystemComponent* ANewBeGinEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}