#include "GAS/Attributes/NewBeGinAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GAS/Characters/NewBeGinEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

UNewBeGinAttributeSet::UNewBeGinAttributeSet()
{
	InitMaxHealth(100.f);
	InitHealth(100.f);
	InitDamage(0.f);
}

void UNewBeGinAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNewBeGinAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNewBeGinAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNewBeGinAttributeSet, Damage, COND_None, REPNOTIFY_Always);
}

void UNewBeGinAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNewBeGinAttributeSet, Health, OldValue);
}

void UNewBeGinAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNewBeGinAttributeSet, MaxHealth, OldValue);
}

void UNewBeGinAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNewBeGinAttributeSet, Damage, OldValue);
}

void UNewBeGinAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void UNewBeGinAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Health 改变后：Clamp
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float NewHealth = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
		SetHealth(NewHealth);
	}

	// MaxHealth 改变后：Clamp
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		const float NewMax = FMath::Max(GetMaxHealth(), 1.f);
		SetMaxHealth(NewMax);

		const float NewHealth = FMath::Clamp(GetHealth(), 0.f, NewMax);
		SetHealth(NewHealth);
	}

	// Damage 改变后：把 Damage 转成扣血
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();

		// Damage 是临时值，立刻清零
		SetDamage(0.f);

		if (LocalDamage > 0.f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.f, GetMaxHealth());
			SetHealth(NewHealth);

			UE_LOG(LogTemp, Warning, TEXT("[GASDamage] Health reduced by %.1f -> %.1f / %.1f"),
				LocalDamage,
				NewHealth,
				GetMaxHealth());

			// 如果血量归零，尝试让拥有者死亡
			if (NewHealth <= 0.f)
			{
				if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
				{
					if (AActor* AvatarActor = ASC->GetAvatarActor())
					{
						if (ANewBeGinEnemyCharacter* Enemy = Cast<ANewBeGinEnemyCharacter>(AvatarActor))
						{
							Enemy->Die();
						}
					}
				}
			}
		}
	}
}