// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"          // ✅ GAS 接口
#include "Logging/LogMacros.h"
#include "Animation/AnimMontage.h"
#include "NewBeGinCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

// ✅ GAS 前置声明
class UAbilitySystemComponent;
class UNewBeGinAbilitySystemComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 *
 *  ✅ GAS 规范：Character 作为 Avatar；ASC 放在 PlayerState
 */
UCLASS(abstract)
class NEWBEGIN_API ANewBeGinCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:

	virtual void BeginPlay() override;

	// =========================
	// 输入（你原来的）
	// =========================

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* AttackMontage;

public:
	/** Constructor */
	ANewBeGinCharacter();

protected:
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void StartAim();
	void StopAim();

	void Attack(const FInputActionValue& Value);

public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** 处理攻击命中（你原来的）*/
	UFUNCTION(BlueprintCallable, Category="Combat")
	void DoAttackHitTrace();

public:
	// =========================
	// 你原来的攻击窗口逻辑
	// =========================

	// 当前这一刀的唯一编号（每次按左键 +1）
	UPROPERTY(VisibleAnywhere, Category="Combat")
	int32 AttackId = 0;

	// 这一刀是否已经造成过伤害
	UPROPERTY(VisibleAnywhere, Category="Combat")
	bool bDidDamageThisAttack = false;

	UFUNCTION(BlueprintCallable, Category="Combat")
	void BeginAttackWindow();

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool CanApplyDamageThisAttack() const;

	UFUNCTION(BlueprintCallable, Category="Combat")
	void MarkDamageApplied();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// =========================
	// ✅ GAS：IAbilitySystemInterface
	// =========================

public:
	/** GAS 通过这个接口拿到 ASC（ASC 实际在 PlayerState） */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	/** 服务器：被 Controller 占有时调用 */
	virtual void PossessedBy(AController* NewController) override;

	/** 客户端：PlayerState 复制完成时调用（联机必备） */
	virtual void OnRep_PlayerState() override;

	/** 统一初始化入口：把 PlayerState 的 ASC 绑定到自己作为 Avatar */
	void InitGAS();

protected:
	/** 缓存 PlayerState 里的 ASC（不在 Character 创建） */
	UPROPERTY()
	TObjectPtr<UNewBeGinAbilitySystemComponent> AbilitySystemComponent;
};