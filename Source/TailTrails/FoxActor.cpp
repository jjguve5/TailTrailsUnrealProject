// Fill out your copyright notice in the Description page of Project Settings.


#include "FoxActor.h"


// Sets default values
AFoxActor::AFoxActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Root Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Initialize the Skeletal Mesh
	FoxMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FoxMesh"));
	FoxMesh->SetupAttachment(RootComponent);
	FoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FoxMesh->SetCollisionProfileName(TEXT("NoCollision"));
	FoxMesh->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void AFoxActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (FoxMeshAsset)
	{
		FoxMesh->SetSkeletalMesh(FoxMeshAsset);
		FoxMesh->SetRelativeLocation(FVector(0, 0, 0));
		FoxMesh->SetRelativeRotation(FRotator(0, 90, 0));
		FoxMesh->SetRelativeScale3D(FVector(1, 1, 1));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FoxMeshAsset is not set!"));
	}
}

// Called every frame
void AFoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UMaterialInterface* AFoxActor::GetMaterialByName(const FString& MaterialName)
{
	FString MaterialPath = FString::Printf(TEXT("/Game/Shared/Assets/Characters/Fox/Materials/%s.%s"), *MaterialName, *MaterialName);
	UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPath));

	if (!Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("Material %s not found! Path: %s"), *MaterialName, *MaterialPath);
	}

	return Material;
}

void AFoxActor::SetMaterialByName(const FString& MaterialName)
{
	UMaterialInterface* Material = GetMaterialByName(MaterialName);

	if (Material && FoxMesh)
	{
		FoxMesh->SetMaterial(0, Material);
		UE_LOG(LogTemp, Warning, TEXT("Material %s set!"), *Material->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set material %s"), *MaterialName);
	}
}
