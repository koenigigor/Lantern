// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lantern.generated.h"

class UPointLightComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLanternDestroyDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanternToggleDelegate, bool, bEnabled);

USTRUCT(BlueprintType)
struct FLanternParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Intensity = 5000.f;
};

UCLASS()
class TASKNETLAMP_API ALantern : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Setup")
	bool bEnabledByDefault = true;

	UPROPERTY(EditAnywhere, Category = "Setup")
	FLanternParams EnabledParams = {FLinearColor::White, 5000.f};

	UPROPERTY(EditAnywhere, Category = "Setup")
	FLanternParams DisabledParams = {FLinearColor::Red, 30.f};

public:
	ALantern();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	bool IsDestroyed() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ToggleLight();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void EnableLight(bool bNowEnabled);

	UPROPERTY(BlueprintAssignable, Transient)
	FOnLanternDestroyDelegate OnLanternDestroyDelegate;

	UPROPERTY(BlueprintAssignable, Transient)
	FOnLanternToggleDelegate OnLanternToggleDelegate;

protected:
	void EnableLight_Internal(bool bNowEnabled);

	void DestroyLantern();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnLightEnabled")
	void BP_OnLightEnabled();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnLightDisabled")
	void BP_OnLightDisabled();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnLanternDestroy")
	void BP_OnLanternDestroy();

	UPROPERTY(ReplicatedUsing="OnRep_Destroyed")
	bool bDestroyed = false;

	UPROPERTY(ReplicatedUsing="OnRep_Enabled")
	bool bLightEnabled = true;

	UFUNCTION()
	void OnRep_Destroyed();

	UFUNCTION()
	void OnRep_Enabled();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPointLightComponent* PointLight = nullptr;
};
