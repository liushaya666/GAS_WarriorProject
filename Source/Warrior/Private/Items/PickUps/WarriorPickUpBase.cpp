// Learning Project


#include "Items/PickUps/WarriorPickUpBase.h"

#include "Components/SphereComponent.h"

// Sets default values
AWarriorPickUpBase::AWarriorPickUpBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
    PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap);   
}

void AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
}


