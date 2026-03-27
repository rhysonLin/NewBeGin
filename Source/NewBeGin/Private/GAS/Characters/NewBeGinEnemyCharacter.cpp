#include "GAS/Characters/NewBeGinEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GAS/ASC/NewBeGinAbilitySystemComponent.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		// 对敌人：
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

void ANewBeGinEnemyCharacter::Die()
{
	// 防止重复死亡
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	UE_LOG(LogTemp, Warning, TEXT("[EnemyDeath] %s died"), *GetName());

	// 1. 关闭胶囊碰撞
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	}

	// 2. 停止移动
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
		MoveComp->StopMovementImmediately();
	}

	// 3. 先简单隐藏（后面你可以改成播放死亡动画）
	SetActorHiddenInGame(true);

	// 4. 延迟销毁，方便你看到日志
	SetLifeSpan(2.0f);
}