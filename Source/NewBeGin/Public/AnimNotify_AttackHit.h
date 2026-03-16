#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackHit.generated.h"

/**
 * 近战攻击命中通知
 * 当动画播放到指定帧时触发命中检测
 */
UCLASS()
class NEWBEGIN_API UAnimNotify_AttackHit : public UAnimNotify
{
	GENERATED_BODY()

public:

	/** 判定球半径 */
	UPROPERTY(EditAnywhere, Category="Attack|Trace")
	float Radius = 60.f;

	/** 向前扫掠距离 */
	UPROPERTY(EditAnywhere, Category="Attack|Trace")
	float ForwardDistance = 120.f;

	/** 起点向前偏移 */
	UPROPERTY(EditAnywhere, Category="Attack|Trace")
	float StartForwardOffset = 80.f;

	/** 造成伤害 */
	UPROPERTY(EditAnywhere, Category="Attack|Damage")
	float Damage = 20.f;

	/** 是否绘制调试图形 */
	UPROPERTY(EditAnywhere, Category="Attack|Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, Category="Attack|Damage")
	bool bHitOnlyOnce = true;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};