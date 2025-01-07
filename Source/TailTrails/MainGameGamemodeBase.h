// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "MainGameGamemodeBase.generated.h"

/**
 * 
 */
UCLASS()
class TAILTRAILS_API AMainGameGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	TMap<APlayerController*, FString> PlayerTokens;

	const FString ApiBaseUrl = TEXT("http://198.7.113.75:3000/api");
	const FString GetPlayerDataUrl = ApiBaseUrl + TEXT("/users/steam");
	const FString GetPlayerItems = ApiBaseUrl + TEXT("/items/user");
	const FString DressItemUrl = ApiBaseUrl + TEXT("/items/dress");
	const FString UnDressItemUrl = ApiBaseUrl + TEXT("/items/undress");
	const FString PurchaseItemUrl = ApiBaseUrl + TEXT("/items/purchase");

public:
	void StorePlayerToken(APlayerController* PlayerController, const FString& Token);
	FString GetPlayerToken(APlayerController* PlayerController) const;

	void OnPlayerDataReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController);
	void OnPlayerItemsReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController);
	void OnDressItemReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController, int32 ItemID);
	void OnUnDressItemReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController, int32 ItemID);
	void OnPurchaseItemReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController, int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void FetchAndStorePlayerData(APlayerController* PlayerController);

	UFUNCTION()
	void HandleInventoryRequest(APlayerController* PlayerController);

	UFUNCTION()
	void HandleDressItemRequest(APlayerController* PlayerController, int32 ItemID);

	UFUNCTION()
	void HandleUnDressItemRequest(APlayerController* PlayerController, int32 ItemID);

	UFUNCTION()
	void HandlePurchaseItemRequest(APlayerController* PlayerController, int32 ItemID);

	UFUNCTION()
	void HandleChatMessage(APlayerController* PlayerController, const FString& Message);
};
