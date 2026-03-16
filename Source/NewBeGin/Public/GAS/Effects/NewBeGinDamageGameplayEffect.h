#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "NewBeGinDamageGameplayEffect.generated.h"

/**
 * 近战伤害 GameplayEffect
 * 真正伤害值通过 SetByCaller 传入
 */
UCLASS()
class NEWBEGIN_API UNewBeGinDamageGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UNewBeGinDamageGameplayEffect();
};