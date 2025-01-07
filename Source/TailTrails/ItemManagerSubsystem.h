// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "ItemManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Color,
	Hat,
	Shirt,
	Pants,
	Shoes
};

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EItemType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Cost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, FString> Properties;
};

/**
 * 
 */
UCLASS()
class TAILTRAILS_API UItemManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	void SendRequest(FString Url, FString Verb, FString Content, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> OnResponseReceived);

	UFUNCTION(BlueprintCallable)
	TArray<FItem> GetItemsByType(EItemType Type) const;

	UFUNCTION(BlueprintCallable)
	FItem GetItemByID(int32 ID) const;

	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetMaterialByName(const FString& MaterialName);

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetIconByItemId(int32 ID) const;

	UFUNCTION(BlueprintCallable)
	USkeletalMesh* GetMeshByItemId(int32 ID) const;

private:
	TArray<FItem> Items;
	const FString ApiBaseUrl = TEXT("http://198.7.113.75:3000/api");
	const FString GetAllItemsUrl = ApiBaseUrl + TEXT("/items/all");

	void LoadItems();
};
