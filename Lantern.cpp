// Fill out your copyright notice in the Description page of Project Settings.


#include "Lantern.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


ALantern::ALantern()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = Collision;

	PointLight = CreateDefaultSubobject<UPointLightComponent>("Light");
	PointLight->SetupAttachment(Collision);
}

void ALantern::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bDestroyed);
	DOREPLIFETIME(ThisClass, bLightEnabled);
}

void ALantern::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	EnableLight_Internal(bEnabledByDefault);
}

bool ALantern::IsDestroyed() const
{
	return bDestroyed;
}


float ALantern::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f) DestroyLantern();

	return ActualDamage;
}


void ALantern::ToggleLight()
{
	if (HasAuthority())
		EnableLight_Internal(!bLightEnabled);
}

void ALantern::EnableLight(bool bNowEnabled)
{
	if (HasAuthority())
		EnableLight_Internal(bNowEnabled);
}

void ALantern::EnableLight_Internal(bool bNowEnabled)
{
	if (bNowEnabled && IsDestroyed()) return;

	bLightEnabled = bNowEnabled;

	//apply light parameters
	const auto& Params = bNowEnabled ? EnabledParams : DisabledParams;
	PointLight->SetLightColor(Params.Color);
	PointLight->SetIntensity(Params.Intensity);

	//send notify only if lantern live
	if (!IsDestroyed())
	{
		bLightEnabled ? BP_OnLightEnabled() : BP_OnLightDisabled();
		OnLanternToggleDelegate.Broadcast(bLightEnabled);
	}
}


void ALantern::DestroyLantern()
{
	EnableLight(false);
	SetCanBeDamaged(false);
	bDestroyed = true;

	BP_OnLanternDestroy();
	OnLanternDestroyDelegate.Broadcast();
}


void ALantern::OnRep_Destroyed()
{
	DestroyLantern();
}

void ALantern::OnRep_Enabled()
{
	EnableLight_Internal(bLightEnabled);
}
