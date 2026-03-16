// Copyright Epic Games, Inc. All Rights Reserved.

#include "NewBeGinCharacter.h"

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "NewBeGin.h"

// ✅ GAS include
#include "AbilitySystemComponent.h"
#include "GAS/PlayerState/NewBeGinPlayerState.h"
#include "GAS/Attributes/NewBeGinAttributeSet.h"
#include "GAS/ASC/NewBeGinAbilitySystemComponent.h"

ANewBeGinCharacter::ANewBeGinCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ANewBeGinCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay Pawn=%s  Class=%s  Controller=%s"),
		*GetName(),
		*GetClass()->GetName(),
		*GetNameSafe(GetController()));

	UE_LOG(LogTemp, Warning, TEXT("AttackAction=%s  AttackMontage=%s"),
		*GetNameSafe(AttackAction),
		*GetNameSafe(AttackMontage));

	// ✅ 单机有时 BeginPlay 时 PlayerState 还没就绪，真正的绑定在 PossessedBy/OnRep_PlayerState 做
}

void ANewBeGinCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANewBeGinCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ANewBeGinCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANewBeGinCharacter::Look);

		// Aiming
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ANewBeGinCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ANewBeGinCharacter::StopAim);
		}

		// Attack
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ANewBeGinCharacter::Attack);
		}
	}
	else
	{
		UE_LOG(LogNewBeGin, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ANewBeGinCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void ANewBeGinCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ANewBeGinCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ANewBeGinCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ANewBeGinCharacter::DoJumpStart()
{
	Jump();
}

void ANewBeGinCharacter::DoJumpEnd()
{
	StopJumping();
}

void ANewBeGinCharacter::StartAim()
{
	UE_LOG(LogNewBeGin, Log, TEXT("StartAim"));

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = 300.0f;
	}
}

void ANewBeGinCharacter::StopAim()
{
	UE_LOG(LogNewBeGin, Log, TEXT("StopAim"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = 400.0f;
	}
}

void ANewBeGinCharacter::Attack(const FInputActionValue& Value)
{
	UE_LOG(LogNewBeGin, Log, TEXT("Attack Pressed!"));

	if (!AttackMontage)
	{
		UE_LOG(LogNewBeGin, Warning, TEXT("AttackMontage is NULL"));
		return;
	}

	if (!GetMesh())
	{
		UE_LOG(LogNewBeGin, Warning, TEXT("Mesh is NULL"));
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogNewBeGin, Warning, TEXT("AnimInstance is NULL"));
		return;
	}

	// ✅ 防止连点导致同一个蒙太奇重复播放
	if (AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		UE_LOG(LogNewBeGin, Log, TEXT("Attack blocked: montage is already playing"));
		return;
	}

	// ✅ 新的一刀：开启攻击窗口（清空本刀已伤害标记）
	BeginAttackWindow();

	const float PlayLen = AnimInstance->Montage_Play(AttackMontage, 1.0f);
	UE_LOG(LogNewBeGin, Warning, TEXT("Montage_Play returned: %f"), PlayLen);
}

void ANewBeGinCharacter::DoAttackHitTrace()
{
	UE_LOG(LogTemp, Warning, TEXT("DoAttackHitTrace called!"));
}

// =========================
// 你原来的攻击窗口逻辑
// =========================
void ANewBeGinCharacter::BeginAttackWindow()
{
	AttackId++;
	bDidDamageThisAttack = false;
}

bool ANewBeGinCharacter::CanApplyDamageThisAttack() const
{
	return !bDidDamageThisAttack;
}

void ANewBeGinCharacter::MarkDamageApplied()
{
	bDidDamageThisAttack = true;
}

// =========================
// ✅ GAS：PlayerState 架构绑定
// =========================

UAbilitySystemComponent* ANewBeGinCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ANewBeGinCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// ✅ 服务器端：Controller 占有角色时，初始化 GAS
	InitGAS();
}

void ANewBeGinCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// ✅ 客户端：PlayerState 复制过来时，初始化 GAS（联机必备）
	InitGAS();
}

void ANewBeGinCharacter::InitGAS()
{
	ANewBeGinPlayerState* PS = GetPlayerState<ANewBeGinPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS] InitGAS failed: PlayerState is null"));
		return;
	}

	UNewBeGinAbilitySystemComponent* ASC = PS->GetASC();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS] InitGAS failed: ASC is null"));
		return;
	}

	AbilitySystemComponent = ASC;

	// ✅ 最正规：Owner=PlayerState，Avatar=Character
	AbilitySystemComponent->NB_InitAbilityActorInfo(PS, this);
	UE_LOG(LogTemp, Warning, TEXT("[GAS] InitGAS OK: Owner=%s Avatar=%s ASC=%s"),
		*GetNameSafe(PS),
		*GetNameSafe(this),
		*GetNameSafe(AbilitySystemComponent));

	// ✅ 初始化基础属性
	if (UNewBeGinAttributeSet* AS = PS->GetAttributeSet())
	{
		AbilitySystemComponent->SetNumericAttributeBase(AS->GetMaxHealthAttribute(), 100.f);
		AbilitySystemComponent->SetNumericAttributeBase(AS->GetHealthAttribute(), 100.f);

		UE_LOG(LogTemp, Warning, TEXT("[GAS] Init Attributes: Health=%.1f MaxHealth=%.1f"),
			AS->GetHealth(),
			AS->GetMaxHealth());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS] Init Attributes failed: AttributeSet is null"));
	}
}