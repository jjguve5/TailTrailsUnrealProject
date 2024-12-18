// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainGamePlayerstateBase.h"
#include "Blueprint/UserWidget.h"
#include "MainGamePlayercontrollerBase.generated.h"

/**
 * 
 */
UCLASS()
class TAILTRAILS_API AMainGamePlayercontrollerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendPlayerToken(const FString& Token);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void CloseInventory();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void CloseCatalog();

	// Function to request inventory from GameMode
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestInventory();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDressItem(int32 ItemID);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUnDressItem(int32 ItemID);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPurchaseItem(int32 ItemID);

	// Function to receive items from the server
	UFUNCTION(Client, Reliable)
	void ClientReceivePlayerItems(const TArray<int32>& PlayerItems, const TArray<int32>& DressedItems);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RequestInventory();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void DressItem(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void UnDressItem(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void PurchaseItem(int32 ItemID);

protected:
	void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	void OpenInventory();
	void OpenCatalog();

	bool bIsInventoryOpen = false;

	UPROPERTY()
	UUserWidget* InventoryWidget; // Pointer to the Inventory Widget

	// Widget Blueprint class reference
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	bool bIsCatalogOpen = false;

	UPROPERTY()
	UUserWidget* CatalogWidget; // Pointer to the Inventory Widget

	// Widget Blueprint class reference
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CatalogWidgetClass;
};
