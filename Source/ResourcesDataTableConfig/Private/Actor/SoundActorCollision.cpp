// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundActorCollision.h"
#include "Components/SphereComponent.h"

ASoundActorCollision::ASoundActorCollision()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetLineThickness(2.5f);
	SphereComponent->ShapeColor = FColor::Green;
	SphereComponent->SetSphereRadius(SphereRadius);
}

#if WITH_EDITOR
void ASoundActorCollision::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//拿到改变的属性

	if (Property->GetFName() == "SphereRadius")
	{
		SphereComponent->SetSphereRadius(SphereRadius);
	}
}
#endif

void ASoundActorCollision::BeginPlay()
{
	Super::BeginPlay();

	if (SphereComponent)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASoundActorCollision::OnBeginOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASoundActorCollision::OnEndOverlap);
	}
}

void ASoundActorCollision::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ASoundActorCollision::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
