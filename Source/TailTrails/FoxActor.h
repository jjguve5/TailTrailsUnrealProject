// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoxActor.generated.h"

UCLASS()
class TAILTRAILS_API AFoxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFoxActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	USkeletalMesh* FoxMeshAsset;
	USkeletalMeshComponent* FoxMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetMaterialByName(const FString& MaterialName);

private:
	UMaterialInterface* GetMaterialByName(const FString& MaterialName);
};
