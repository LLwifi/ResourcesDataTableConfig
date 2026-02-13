// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RDTC_DecalFootsteps.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ARDTC_DecalFootsteps::ARDTC_DecalFootsteps()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionTransform.SetRotation(FQuat(FRotator(-90.0f,0.0f,0.0f)));

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(SceneComponent);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>("DecalComponent");
	DecalComponent->SetupAttachment(RootComponent);
	DecalComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

}

#if WITH_EDITOR
void ARDTC_DecalFootsteps::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ARDTC_DecalFootsteps, DecalMaterialInterface))
	{
		DecalComponent->SetDecalMaterial(DecalMaterialInterface);
	}
	else if(Property->GetFName() == GET_MEMBER_NAME_CHECKED(ARDTC_DecalFootsteps, PhysMaterial))
	{
		UPrimitiveComponent* CollisionCom = GetComponentByClass<UPrimitiveComponent>();
		if (CollisionCom)
		{
			CollisionCom->SetPhysMaterialOverride(PhysMaterial);
		}
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ARDTC_DecalFootsteps, bIsAutoAlign) || Property->GetFName() == GET_MEMBER_NAME_CHECKED(ARDTC_DecalFootsteps, CollisionTransform))
	{
		UPrimitiveComponent* CollisionCom = GetComponentByClass<UPrimitiveComponent>();
		if (CollisionCom)
		{
			if (bIsAutoAlign)
			{
				AutoAlignCollision();
			}
			else
			{
				CollisionCom->SetRelativeTransform(CollisionTransform);
			}
		}
	}
}
#endif

// Called when the game starts or when spawned
void ARDTC_DecalFootsteps::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARDTC_DecalFootsteps::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARDTC_DecalFootsteps::AutoAlignCollision()
{
	if (bIsAutoAlign)
	{
		UPrimitiveComponent* CollisionCom = GetComponentByClass<UPrimitiveComponent>();
		if (CollisionCom)
		{
			CollisionCom->SetRelativeTransform(DecalComponent->GetRelativeTransform());
			UBoxComponent* Box = Cast<UBoxComponent>(CollisionCom);
			if (Box)
			{
				Box->SetBoxExtent(DecalComponent->DecalSize);
			}
			else
			{
				USphereComponent* Sphere = Cast<USphereComponent>(CollisionCom);
				if (Sphere)
				{
					Sphere->SetSphereRadius(DecalComponent->DecalSize.Y);
				}
			}
		}
	}
}

