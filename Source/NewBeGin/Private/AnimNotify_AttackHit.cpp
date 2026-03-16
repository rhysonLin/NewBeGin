#include "AnimNotify_AttackHit.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "NewBeGinCharacter.h"
#include "GAS/PlayerState/NewBeGinPlayerState.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"
#include "GAS/Effects/NewBeGinDamageGameplayEffect.h"
#include "GAS/ASC/NewBeGinAbilitySystemComponent.h"

void UAnimNotify_AttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// =========================
	// 1) 安全检查
	// =========================
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	// =========================
	// 2) 计算 Sweep 起点 / 终点
	// =========================
	const FVector Forward = Owner->GetActorForwardVector();

	// 起点：角色位置抬高一点，再向前偏移一点
	const FVector Start =
		Owner->GetActorLocation() +
		FVector(0.f, 0.f, 60.f) +
		Forward * StartForwardOffset;

	// 终点：沿前方扫出去
	const FVector End = Start + Forward * ForwardDistance;

	// =========================
	// 3) Sweep（忽略自己）
	// =========================
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackHit), false, Owner);

	const bool bHit = World->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	// =========================
	// 4) Debug 绘制
	// =========================
	if (bDrawDebug)
	{
		const FColor Color = bHit ? FColor::Red : FColor::Green;
		DrawDebugLine(World, Start, End, Color, false, 0.25f, 0, 1.5f);
		DrawDebugSphere(World, End, Radius, 16, Color, false, 0.25f);
	}

	// =========================
	// 5) 没打到就结束
	// =========================
	if (!bHit || !Hit.GetActor() || Hit.GetActor() == Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackHit] No Hit"));
		return;
	}

	AActor* HitActor = Hit.GetActor();

	// =========================
	// 6) 打到墙 / 地面：只打印
	// =========================
	if (!HitActor->IsA<APawn>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackHit] Hit World: %s"), *HitActor->GetName());
		return;
	}

	// =========================
	// 7) 本次攻击是否已经造成过伤害
	// =========================
	if (ANewBeGinCharacter* Ch = Cast<ANewBeGinCharacter>(Owner))
	{
		if (bHitOnlyOnce && !Ch->CanApplyDamageThisAttack())
		{
			UE_LOG(LogTemp, Warning, TEXT("[AttackHit] Blocked: already damaged this attack"));
			return;
		}
	}

	APawn* HitPawn = Cast<APawn>(HitActor);
	if (!HitPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GASDamage] Hit actor is not a Pawn"));
		return;
	}

	// =========================
	// 8) 找目标 ASC
	// 优先级：
	//   A. 目标自己实现 IAbilitySystemInterface（AI/敌人常见）
	//   B. 从目标的 PlayerState 拿（玩家常见）
	// =========================
	UAbilitySystemComponent* TargetASC = nullptr;

	// A. 目标自己带 ASC
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitActor))
	{
		TargetASC = ASCInterface->GetAbilitySystemComponent();
	}

	// B. 目标自己没有 ASC，则尝试从 PlayerState 获取
	if (!TargetASC)
	{
		if (ANewBeGinPlayerState* TargetPS = HitPawn->GetPlayerState<ANewBeGinPlayerState>())
		{
			TargetASC = TargetPS->GetAbilitySystemComponent();
		}
	}

	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GASDamage] Target ASC is null: %s"), *HitActor->GetName());
		return;
	}

	// =========================
	// 9) 从目标 ASC 上拿 AttributeSet
	// =========================
	UNewBeGinAttributeSet* TargetAS = const_cast<UNewBeGinAttributeSet*>(TargetASC->GetSet<UNewBeGinAttributeSet>());
	if (!TargetAS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GASDamage] Target AttributeSet is null: %s"), *HitActor->GetName());
		return;
	}

	// =========================
	// 10) 直接修改目标 Health（当前阶段的第一版 GAS 扣血）
	// 说明：
	// - 这还不是最终的 GameplayEffect 方案
	// - 但已经进入 GAS Attribute 链路了
	// =========================
	const float OldHealth = TargetAS->GetHealth();

	// 创建 EffectContext
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(Owner);

	// 创建 GE Spec
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(
		UNewBeGinDamageGameplayEffect::StaticClass(),
		1.0f,
		EffectContext
	);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GASDamage] Failed to create SpecHandle"));
		return;
	}

	// 把 Damage 通过 SetByCaller 传进去
	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
		Damage
	);

	// 应用到目标
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// 标记本次攻击已经命中过
	if (ANewBeGinCharacter* Ch = Cast<ANewBeGinCharacter>(Owner))
	{
		if (bHitOnlyOnce)
		{
			Ch->MarkDamageApplied();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[GASDamage] Applied GE to %s | OldHealth=%.1f | Damage=%.1f"),
		*HitActor->GetName(),
		OldHealth,
		Damage);
}