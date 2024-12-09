// Fill out your copyright notice in the Description page of Project Settings.


#include "FoxActor.h"


// Sets default values
AFoxActor::AFoxActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFoxActor::BeginPlay()
{
	Super::BeginPlay();
	
	//spawn the skeletal mesh component at 0 0 0 with z rotation of 90 and mesh fox
	/*SkeletalMesh = NewObject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));
	SkeletalMesh->RegisterComponent();	
	SkeletalMesh->SetWorldLocation(FVector(0, 0, 0));
	SkeletalMesh->SetWorldRotation(FRotator(0, 0, 90));
	SkeletalMesh->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Script/Engine.SkeletalMesh'/Game/Shared/Assets/Characters/Fox/Meshes/Fox.Fox'")));
	SkeletalMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SetRootComponent(SkeletalMesh);*/
}

// Called every frame
void AFoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UMaterial* AFoxActor::GetMaterialByName(const FString& MaterialName)
{
    FString MaterialPath = FString::Printf(TEXT("/Script/Engine.Material'/Game/Shared/Assets/Characters/Fox/Materials/%s.%s'"), *MaterialName, *MaterialName);
    UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *MaterialPath));

    if (!Material)
    {
        UE_LOG(LogTemp, Warning, TEXT("Material %s not found!"), *MaterialName);
    }

    return Material;
}

void AFoxActor::SetMaterialByName(const FString& MaterialName)
{
	UMaterial* Material = GetMaterialByName(MaterialName);

	if (Material)
	{
		//SkeletalMesh->SetMaterial(0, Material);
		UE_LOG(LogTemp, Warning, TEXT("Material %s set!"), *Material->GetName());
	}
}
